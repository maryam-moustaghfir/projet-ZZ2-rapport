#include "header.hpp"

void lire_fichier_instance(t_probleme &P, string nom)
{
    // initialisation

    ifstream fichier(nom); // Ouvre le fichier en lecture
    if (fichier.is_open())
    {
        // Lecture du nombre de jobs et de machines
        fichier >> P.nb_de_job;
        fichier >> P.nb_de_machine;
        // initialisation de matrice G
        for (int i = 0; i < nb_max_job; i++)
        {
            for (int j = 0; j < nb_max_machine; j++)
            {
                P.G[i][j] = -1;
            }
        }
        for (int i = 0; i < nb_max_job * nb_max_machine; i++)
        {
            P.Machines[i] = -1;
            P.duree_des_operation[i] = -1;
        }
        int c = 0;

        // Pour chaque job
        for (int i = 0; i < P.nb_de_job; i++)
        {
            // int d = 0;
            int z = 0;
            int t = 0;
            t_job job;
            fichier >> job.nb_groupes; // Lecture du nombre de groupes d'opérations pour ce job
            // Pour chaque groupe d'opérations
            for (int j = 0; j < job.nb_groupes; j++)
            {
                int nb_operations;
                fichier >> nb_operations; // Lecture du nombre d'opérations dans ce groupe
                P.OP[i][j] = nb_operations;
                vector<t_operation> groupe;

                // Pour chaque opération dans le groupe
                for (int k = 0; k < nb_operations; k++)
                {
                    t_operation operation;
                    operation.numero_operation = c;

                    operation.numJob = i;
                    operation.numGroup = j;

                    // cout << operation.numero_operation <<" " ;
                    fichier >> operation.machine; // Lecture de la machine utilisée pour cette opération
                    P.Machines[operation.numero_operation] = operation.machine;

                    // d++;
                    P.G[i][t] = j;
                    c++;
                    t++;
                    int nb_sous_operations;
                    fichier >> nb_sous_operations; // Lecture du nombre de sous-opérations
                    int duree_operation = 0;
                    // Pour chaque sous-opération
                    for (int l = 0; l < nb_sous_operations; l++)
                    {
                        t_sous_operation sous_operation;
                        fichier >> sous_operation.duree; // Lecture de la durée de la sous-opération
                        duree_operation += sous_operation.duree;
                        fichier >> sous_operation.puissance;                 // Lecture de la puissance de la sous-opération
                        operation.sous_operations.push_back(sous_operation); // Ajout de la sous-opération à l'opération
                    }
                    P.duree_des_operation[operation.numero_operation] = duree_operation;
                    z++;
                    groupe.push_back(operation); // Ajout de l'opération au groupe
                }

                job.groupes_operations.push_back(groupe); // Ajout du groupe au job
            }
            P.jobs.push_back(job); // Ajout du job au problème
        }

        fichier.close(); // Fermeture du fichier
    }
}

void generer_vecteur_bierwith(t_probleme P, t_solution &S)
{
    vector<int> count(P.nb_de_job, P.nb_de_machine);
    int taille_vecteur = P.nb_de_job * P.nb_de_machine;
    S.taille_vecteur_bierwith = taille_vecteur;
    S.vecteur_bierwith.resize(taille_vecteur);

    // generation du vecteur
    int r = 0;
    for (int i = 0; i < taille_vecteur; i++)
    {
        r = rand() % P.nb_de_job;
        while (count[r] <= 0)
        {
            r = rand() % P.nb_de_job;
        }
        S.vecteur_bierwith[i] = r;
        count[r]--;
    }
}
void generer_vecteur_PrecGroup(t_probleme &P, t_solution &S)
{
    // initialisation
    int occurence[nb_max_job] = {0};
    for (int i = 0; i < S.taille_vecteur_bierwith; i++)
    {
        int job_actuelle = S.vecteur_bierwith[i];         // job 0 pour i =0
        int occurence_actuelle = occurence[job_actuelle]; // 0
        occurence[job_actuelle]++;
        int num_groupe = P.G[job_actuelle][occurence_actuelle];

        if (occurence[job_actuelle] == 1 || (num_groupe > 0 && occurence_actuelle == P.OP[job_actuelle][num_groupe - 1]))
        {
            vector<int> machines_groupe_actuel;
            for (auto op : P.jobs[job_actuelle].groupes_operations[num_groupe])
            {
                machines_groupe_actuel.push_back(op.numero_operation);
            }
            for (int k : machines_groupe_actuel)
            {
                S.PrecGroup[job_actuelle][num_groupe].push_back(k);
            }
            if (S.PrecGroup[job_actuelle][num_groupe].size() > 1)
            {
                random_device rd; // Utiliser une graine provenant du matériel si disponible
                mt19937 g(rd());  // Générateur de nombres aléatoires basé sur Mersenne Twister

                shuffle(S.PrecGroup[job_actuelle][num_groupe].begin(),
                        S.PrecGroup[job_actuelle][num_groupe].end(), g);
            }
        }
    }
}

void evaluer_solution(t_probleme P, t_solution &S)
{
    // Initialisation
    for (int i = 0; i < Duree_max; i++)
    {
        S.tab[i] = Puissance;
    }
    // S.date_op[nb_max_job*nb_max_machine]={0};
    int occGroupe[nb_max_job][nb_max_groupe];
    for (int i = 0; i < nb_max_job; i++)
    {
        for (int j = 0; j < nb_max_groupe; j++)
        {
            occGroupe[i][j] = 0;
        }
    }
    int last_op_for_job[nb_max_job] = {0};
    int nb_jobs_actuels[nb_max_job] = {0}, // Tableau pour suivre le nombre d'op�rations d�j� effectu�es pour chaque job
        job = 0,                           // Job en cours
        machine = 0;                       // Machine en cours
    // debut = 0,                         // Date de d�but de l'op�ration en cours
    // fin_operation_machine = 0;        // Date de fin de l'op�ration en cours
    // fin_operation_job = 0,
    // temps_operation = 0;                          // Dur�e de l'op�ration en cours
    int nb_machines_actuelles[nb_max_machine] = {-1}; // Tableau pour suivre les op�rations en cours sur chaque machine
    int group_actuel{0};
    int operation = 0;
    // int debut_job = 0;
    // int debut_machine = 0;
    for (int i = 0; i < nb_max_job * nb_max_machine; i++)
    {
        S.St[i] = 0;
        S.end_date[i] = 0;
    }
    // Init solution S
    for (int i = 0; i < nb_max_job * nb_max_machine; i++)
    {
        S.pere[i] = 0;
    }

    // Parcours

    // cout << fin_operation_machine << " ";
    for (int i = 0; i < P.nb_de_job * P.nb_de_machine; i++)
    {
        job = S.vecteur_bierwith[i]; // S�lectionne le prochain job � traiter
        // machine = P.Machines[job][nb_jobs_actuels[job]]; // S�lectionne la machine pour le job en cours
        group_actuel = P.G[job][nb_jobs_actuels[job]];
        operation = S.PrecGroup[job][group_actuel][occGroupe[job][group_actuel]];
        occGroupe[job][group_actuel]++;
        // cout << operation << " ";
        //  cout <<"----" ;
        machine = P.Machines[operation];
        // cout << machine << " ";
        if (nb_jobs_actuels[job] > 0)
        {
            int fin_job_prec = S.end_date[last_op_for_job[job]]; // R�cup�re la date de d�but de la derni�re op�ration du job
            // int fin_operation_job = debut_job + P.duree_des_operation[last_op_for_job[job]]; // Calcule la date de fin de l'op�ration en cours

            // Mise � jour du parent et de la date au plus tard si n�cessaire
            if (fin_job_prec > S.St[operation])
            {
                S.St[operation] = fin_job_prec; // Met � jour la date de d�but de l'op�ration en cours
                S.pere[operation] = last_op_for_job[job];
            }
        }

        // le cas d'une opération en cours
        if (nb_machines_actuelles[machine] != -1)
        {
            int fin_machine_prec = S.end_date[nb_machines_actuelles[machine]]; // d�but de l'op�ration prec sur m�me machine
            // S.date_op[operation] = debut;
            // int fin_operation_machine = debut_machine + P.duree_des_operation[nb_machines_actuelles[machine]];
            // cout << fin_operation_machine << " ";
            //  Mise � jour de la solution si n�cessaire
            if (fin_machine_prec > S.St[operation])
            {
                S.St[operation] = fin_machine_prec; // Met � jour la date de d�but de l'op�ration en cours
                // S.pere[job][nb_jobs_actuels[job]] = nb_machines_actuelles[machine]; // Met � jour le job parent
                S.pere[operation] = nb_machines_actuelles[machine];
            }
        }

        // decaleroperatyion (operation);
        t_operation &op = find_Operation(P, operation);
        op.dateDebut = S.St[operation];
        DecalerUneOperation(op, S);
        S.St[operation] = op.dateDebutPuissance;
        // InsererOperation(op, S.tab, debut);

        // cout << S.date_op[i] << " " ;
        // S.St[operation] = 1;
        // cout << "dureee  ";
        // cout << P.duree_des_operation[operation] << " ";
        S.end_date[operation] = S.St[operation] + P.duree_des_operation[operation];
        last_op_for_job[job] = operation;
        nb_machines_actuelles[machine] = operation; // Met � jour le job en cours sur la machine
        nb_jobs_actuels[job]++;                     // Incr�mente le nombre d'op�rations effectu�es pour le job en cours
    }

    S.cout = 0;

    // Recherche du pere du sommet étoile dans le chemin critique
    // cout << " last op for job " << endl;
    for (int i = 0; i < P.nb_de_job; i++)
    {

        // Calcul de la date et de la durée
        // cout <<last_op_for_job[i] << " " ;
        // debut = S.St[last_op_for_job[i]];
        // temps_operation = P.duree_des_operation[last_op_for_job[i]];

        // Si nouveau chemin critique trouvé
        if (S.end_date[last_op_for_job[i]] > S.cout)
        {
            S.cout = S.end_date[last_op_for_job[i]]; // Met à jour le coût de la solution
            S.sommet = last_op_for_job[i];           // Met à jour le rang du sommet étoile
        }
    }
    // cout << "________________________AFFICHAGE Du COUP_________________________" << endl;
    // cout << S.cout << endl;
    // cout << "________________________AFFICHAGE DE PERE_________________________" << endl;
    // for (int i = 0; i < P.nb_de_job * P.nb_de_machine; i++)
    // {

    //     cout << "(" << S.pere[i] << ") ";
    // }
    // cout << endl;
    // cout << "________________________AFFICHAGE DE ST_________________________" << endl;
    // for (int i = 0; i < P.nb_de_job * P.nb_de_machine; i++)
    // {

    //     cout << S.St[i] << " ";
    // }
    // cout << endl;
}

void InsererOperation(t_operation &operation, t_solution &S, int debut)
{
    int t = debut;
    for (auto k : operation.sous_operations)
    {
        for (int i = 0; i < k.duree; i++)
        {
            S.tab[t + i] -= k.puissance;
        }
        t += k.duree;
    }
}

void DecalerUneOperation(t_operation &operation, t_solution &S)
{
    int debut = operation.dateDebut;
    int t = debut,
        b = 0;
    while (debut < Duree_max && b == 0)
    {
        t = debut;
        for (auto sousop : operation.sous_operations)
        {
            for (int i = 0; i < sousop.duree; i++)
            {
                if (S.tab[t + i] >= sousop.puissance)
                {
                    b = 1;
                }
                else
                {
                    debut++;
                    b = 0;
                    break;
                }
            }
            if (b == 1)
            {
                t += sousop.duree;
            }
            else
            {
                break;
            }
        }
    }
    operation.dateDebutPuissance = debut;
    InsererOperation(operation, S, debut);
}

void PermutationJob(t_solution &S)
{
    int indice1 = rand() % S.taille_vecteur_bierwith;
    int indice2 = rand() % S.taille_vecteur_bierwith;
    while (indice1 == indice2)
    {
        indice2 = rand() % (S.taille_vecteur_bierwith);
    }
    // Échanger les éléments correspondants
    int temp = S.vecteur_bierwith[indice1];
    S.vecteur_bierwith[indice1] = S.vecteur_bierwith[indice2];
    S.vecteur_bierwith[indice2] = temp;
}

void PermuterOperation(t_probleme P, t_solution &S)
{
    int stop{0};
    while (stop == 0)
    {

        int j = rand() % (P.nb_de_job);
        int g = rand() % (P.G[j][P.nb_de_machine - 1] + 1);
        if (S.PrecGroup[j][g].size() > 1)
        {
            int indice1 = rand() % S.PrecGroup[j][g].size();
            int indice2 = rand() % S.PrecGroup[j][g].size();
            while (indice1 == indice2)
            {
                indice2 = rand() % (S.PrecGroup[j][g].size());
            }

            int temp = S.PrecGroup[j][g][indice1];
            S.PrecGroup[j][g][indice1] = S.PrecGroup[j][g][indice2];
            S.PrecGroup[j][g][indice2] = temp;
            stop = 1;
        }
        else
        {
            stop = 0;
        }
    }
}
void recherche_locale(t_probleme P, t_solution &S, int nb_iter_max)
{

    t_solution nouvelleSolution;
    int iter = 0;
    double nombreAleatoire = 0.0;
    while (iter < nb_iter_max)
    {
        nombreAleatoire = static_cast<double>(rand()) / RAND_MAX;
        nouvelleSolution = S;
        if (nombreAleatoire < 0.5)
        {
            PermuterOperation(P, nouvelleSolution);
        }
        else
        {
            PermutationJob(nouvelleSolution);
        }
        evaluer_solution(P, nouvelleSolution);
        if (nouvelleSolution.cout < S.cout)
        {
            S = nouvelleSolution;
        }
        iter++;
    }
}
void permutation(t_solution &S1, t_solution &S2)
{
    // choix aleatoire de 2 jobs
    int indice1 = rand() % (S2.taille_vecteur_bierwith);
    int indice2 = rand() % (S2.taille_vecteur_bierwith);

    while (indice1 == indice2)
    {
        indice2 = rand() % (S2.taille_vecteur_bierwith);
    }
    // echange de leur position
    S1.vecteur_bierwith[indice1] = S2.vecteur_bierwith[indice2];
    S1.vecteur_bierwith[indice2] = S2.vecteur_bierwith[indice1];
}
void permutationGroup(t_probleme &P, t_solution &S1, t_solution &S2)
{
    int stop{0};
    while (stop == 0)
    {
        int j = rand() % (P.nb_de_job);
        int g = rand() % (P.G[j][P.nb_de_machine - 1] + 1);
        if (S1.PrecGroup[j][g].size() > 1)
        {
            int indice1 = rand() % S1.PrecGroup[j][g].size();
            int indice2 = rand() % S1.PrecGroup[j][g].size();
            while (indice1 == indice2)
            {
                indice2 = rand() % (S1.PrecGroup[j][g].size());
            }

            // int temp = S.PrecGroup[j][g][indice1];
            S1.PrecGroup[j][g][indice1] = S2.PrecGroup[j][g][indice2];
            S1.PrecGroup[j][g][indice2] = S2.PrecGroup[j][g][indice1];
            stop = 1;
        }
        else
        {
            stop = 0;
        }
    }
}

void graspELS(t_probleme P, t_solution &S, int nb_iter_max, int nb_sous_iter, int nb_de_voisins, int nb_iter_recherche_local)
{
    // intialisation

    generer_vecteur_bierwith(P, S); // generer un vecteur de Bierwith aleatoire
    generer_vecteur_PrecGroup(P, S);
    evaluer_solution(P, S);
    t_solution best_solution = S,
               current_best_solution = S,
               neighbor_solution = best_solution;
    cout << "cout avant Grasp : " << S.cout << endl;
    for (auto i = 0; i < nb_iter_max; i++)
    {
        //ft10_1 job shop ft10_2
        generer_vecteur_bierwith(P, S); 
        generer_vecteur_PrecGroup(P, S);
        evaluer_solution(P, S);
        recherche_locale(P, S, nb_iter_recherche_local);
        for (int j = 0; j < nb_sous_iter; j++)
        {
            
            //  current_best_solution = S;
            current_best_solution.cout = 99999999;
            for (auto k = 0; k < nb_de_voisins; k++)
            {
                cout << "voisin : " << k <<endl;
                neighbor_solution = S;
                // neighbor_solution = best_solution
                permutation(neighbor_solution, S);         // generer voisin
                permutationGroup(P, neighbor_solution, S); // generer voisin
                recherche_locale(P, neighbor_solution, nb_iter_recherche_local);
                if (neighbor_solution.cout < current_best_solution.cout)
                {
                    current_best_solution = neighbor_solution;
                }
            }
            S = current_best_solution;
            // current_best_solution = S;
            //best_solution = current_best_solution;
            if (best_solution.cout > S.cout)
            {
                // S = best_solution;
                best_solution = S;
            }
            cout << "sous iterra "<< j << "best solution sous iter "<< best_solution.cout <<endl;
        }
        cout << " iteration : " << i << " cout " << best_solution.cout << endl;
    }
    S = best_solution;
}
t_operation &find_Operation(t_probleme &P, int numero_op)
{

    for (auto &job : P.jobs)
    {
        for (auto &groupe : job.groupes_operations)
        {
            for (auto &operation : groupe)
            {
                if (operation.numero_operation == numero_op)
                {
                    return operation;
                }
            }
        }
    }
    return P.jobs[0].groupes_operations[0][0];
}
void generer_fichier_gantt(t_probleme P, t_solution S, string nom_fichier)
{
    // S.date_op[nb_max_job*nb_max_machine]={0};
    int occGroupe[nb_max_job][nb_max_groupe];
    for (int i = 0; i < nb_max_job; i++)
    {
        for (int j = 0; j < nb_max_groupe; j++)
        {
            occGroupe[i][j] = 0;
        }
    }

    int nb_jobs_actuels[nb_max_job] = {0}, // Tableau pour suivre le nombre d'op�rations d�j� effectu�es pour chaque job
                                           // Job en cours
        machine = 0;                       // Machine en cours
                                           // Tableau pour suivre les op�rations en cours sur chaque machine
    int group_actuel{0};
    int operation = 0;
    ofstream fichier_gantt(nom_fichier); // Ouvre le fichier en �criture
    if (fichier_gantt.is_open())
    {
        // �criture des en-t�tes
        fichier_gantt << "Job\tMachine\tDébut\tFin\n";

        // Parcours des op�rations
        for (int i = 0; i < P.nb_de_job * P.nb_de_machine; i++)
        {
            int job = S.vecteur_bierwith[i];
            group_actuel = P.G[job][nb_jobs_actuels[job]];
            operation = S.PrecGroup[job][group_actuel][occGroupe[job][group_actuel]];
            machine = P.Machines[operation];
            fichier_gantt << job << "\t" << machine << "\t" << S.St[operation] << "\t" << S.end_date[operation] << "\n";
            nb_jobs_actuels[job]++;
            occGroupe[job][group_actuel]++;
        }

        fichier_gantt.close(); // Fermeture du fichier
    }
    else
    {
        cout << "Erreur lors de l'ouverture du fichier pour �criture." << endl;
    }
}

void generer_fichier_power(t_solution S,string nom_fichier)
{
    ofstream fichier(nom_fichier);
    if (fichier.is_open()) {
        // Parcourir le tableau et écrire l'indice et la valeur dans le fichier
        for (int indice = 0; indice < Duree_max; ++indice) {
            fichier << "Indice : " << indice << ", Valeur : " << S.tab[indice] << std::endl;
        }

        // Fermer le fichier
        fichier.close();
        std::cout << "Données écrites dans le fichier avec succès." << std::endl;
    } else {
        // Afficher un message si le fichier n'a pas pu être ouvert
        std::cerr << "Erreur : Impossible d'ouvrir le fichier." << std::endl;
    }

}

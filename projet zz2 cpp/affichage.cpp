#include "affichage.hpp"
void verifier_lecture_instance(t_probleme P)
{
    // Affichage des données lues pour vérification
    cout << "Nombre de jobs : " << P.nb_de_job << endl;
    cout << "Nombre de machines : " << P.nb_de_machine << endl;

    for (int i = 0; i < P.nb_de_job; i++)
    {
        t_job job = P.jobs[i];
        cout << "Job " << i + 1 << " : " << endl;
        cout << "Nombre de groupes d'operations : " << job.nb_groupes << endl;

        for (int j = 0; j < job.nb_groupes; j++)
        {
            vector<t_operation> groupe = job.groupes_operations[j];
            cout << "Groupe " << j + 1 << " : " << endl;

            for (vector<t_operation>::size_type k = 0; k < groupe.size(); k++)
            {
                t_operation operation = groupe[k];
                cout << "Operation " << k + 1 << " : Machine " << operation.machine << endl;

                for (vector<t_sous_operation>::size_type l = 0; l < operation.sous_operations.size(); l++)
                {
                    t_sous_operation sous_operation = operation.sous_operations[l];
                    cout << "    Sous-operation " << l + 1 << " : Duree " << sous_operation.duree << ", Puissance " << sous_operation.puissance << endl;
                }
            }
        }
    }
}
void afficher_Bierwith(t_solution &S)
{
    cout << S.taille_vecteur_bierwith << endl;
    for (int i = 0; i < S.taille_vecteur_bierwith; i++)
    {
        cout << S.vecteur_bierwith[i] << " ";
    }
    cout << endl;
}
void afficher_PrecGroup(t_solution &S)
{
    for (int i = 0; i < nb_max_job; i++)
    {
        for (int j = 0; j < nb_max_groupe; j++)
        {
            cout << "[";
            for (auto k : S.PrecGroup[i][j])
            {
                cout << k << " ";
            }
            cout << "]";
        }
        cout << endl;
    }
}

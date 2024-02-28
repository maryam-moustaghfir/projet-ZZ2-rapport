#include <iostream>
#include "header.hpp"
#include "affichage.hpp"
using namespace std;
int main()
{
    srand(time(0));
    t_probleme probleme;
    t_solution solution;
    // "abz7_14.txt","abz7_9.txt",
    //vector<string> file = {"ft10_5.txt","ft10_9.txt","la38_8.txt","la38_13.txt"};
    /*for (auto f : file)
    {
        //srand(time(0));
        //string nom_fichier = f.append(".txt");
        //cout << nom_fichier << " ";
        lire_fichier_instance(probleme, f);
        cout << "instance : " << f <<endl;
        grasp(probleme, solution, 10, 10, 10);
        cout << "nouveau cout Grasp : " << solution.cout << endl;
    }*/
    // la38_3 abz7_4 abz7_7
    
    string nom_fichier = "ft10_9.txt";
    lire_fichier_instance(probleme, nom_fichier);
    // verifier_lecture_instance(probleme);
    //generer_vecteur_bierwith(probleme, solution);
    // afficher_Bierwith(solution);
    //generer_vecteur_PrecGroup(probleme, solution);
    // afficher_PrecGroup(solution);
    //evaluer_solution(probleme, solution);
    //cout << "ancien cout rech: " << solution.cout << endl;
    //recherche_locale(probleme, solution, 50000);
    //cout << "nouveau cout rech: " << solution.cout << endl;
    cout << "instance : " << nom_fichier << endl;
    graspELS(probleme, solution, 3, 5, 10,1000);
    cout << "le cout apres Grasp : " << solution.cout << endl;
    generer_fichier_gantt(probleme,solution,"Gantt "+nom_fichier);
    generer_fichier_power(solution,"power"+nom_fichier);

    return 0;
}

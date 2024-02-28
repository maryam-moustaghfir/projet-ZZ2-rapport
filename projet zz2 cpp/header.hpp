#ifndef HEADER_H
#define HEADER_H

#include <string>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <random>
#include <algorithm>
using namespace std;

const int nb_max_job = 30,
          nb_max_machine = 30,
          nb_max_groupe = 30,
          Duree_max = 100000, //100000
          Puissance = 2000;

// déclaration des structures
typedef struct t_sous_operation
{
    int duree;
    int puissance;
} t_sous_operation;

typedef struct t_operation
{
    int numero_operation;
    int numJob;
    int numGroup;
    int machine;
    int dateDebut;
    int dateDebutPuissance;
    vector<t_sous_operation> sous_operations; // Les sous-opérations pour chaque opération
} t_operation;

typedef struct t_job
{
    int nb_groupes;
    vector<vector<t_operation>> groupes_operations; // Les groupes d'opérations pour chaque job
} t_job;

typedef struct t_probleme
{
    int nb_de_job;
    int nb_de_machine;
    int duree_des_operation[nb_max_job * nb_max_machine];
    // int duree_des_operation[nb_max_job][nb_max_machine]; //duree_des_operation[1][1] = duree sur l'op 1
    vector<t_job> jobs;
    int G[nb_max_job][nb_max_machine]; // G[numJob][occurence_bierwith] = numGroup | G[0][0] = 0
    int OP[nb_max_job][nb_max_groupe];
    int Machines[nb_max_job * nb_max_machine];
} t_probleme;
// typedef struct t_couple
// {
//     // int job;
//     // int groupe;
//     // int operation;
//     int numero_de_piece;
//     int rang_dans_la_gamme;
// } t_couple;

typedef struct t_solution
{
    int cout;
    int taille_vecteur_bierwith;
    vector<int> vecteur_bierwith; // vecteur_bierwith[i=0]= 1 (job 1)
    // PrecGroup[0][0][0] = 0 (job 1 group 1 occurence 1 machine 0)
    vector<int> PrecGroup[nb_max_job][nb_max_groupe]; // PrecGroup[numJob][G[numJob][occurence_bierwith]][occurence_bierwith] = numMachine
    // t_couple pere[nb_max_job][nb_max_machine];
    // int pere[nb_max_job][nb_max_machine];
    // t_couple sommet;
    // int St[nb_max_job*nb_max_machine];
    // int St[nb_max_job][nb_max_machine];
    // int Last_operation_job[nb_max_job][nb_max_job * nb_max_machine];
    // int enddate[nb_max_job * nb_max_machine];
    int pere[nb_max_job * nb_max_machine];
    int sommet;
    int St[nb_max_job * nb_max_machine];
    int end_date[nb_max_job * nb_max_machine];
    int last_op_for_job[nb_max_job];
    int tab[Duree_max];
} t_solution;

// ---------------lecture fichier--------------------
void lire_fichier_instance(t_probleme &P, string nom);
// ----------------------------------------------------

// --------------------Vecteurs--------------------
void generer_vecteur_bierwith(t_probleme P, t_solution &S);

void generer_vecteur_PrecGroup(t_probleme &P, t_solution &S);
// ----------------------------------------------------

// ----------------evaluerPuissance---------------------
t_operation &find_Operation(t_probleme &P, int numero_op);

void InsererOperation(t_operation &operation, t_solution &S, int debut);

void DecalerUneOperation(t_operation &operation, t_solution &S);

void evaluer_solution(t_probleme P, t_solution &S);
// ------------------------------------------------

// ----------Recherche locale---------------------
void PermutationJob(t_solution &S);

void PermuterOperation(t_probleme P, t_solution &S);

void recherche_locale(t_probleme P, t_solution &S, int nb_iter_max);
// -----------------------------------------------

// ---------------------GRASP---------------------
void permutation(t_solution &S1, t_solution &S2);

void permutationGroup(t_probleme &P, t_solution &S1, t_solution &S2);

void graspELS(t_probleme P, t_solution &S, int nb_iter_max,int nb_sous_iter, int nb_de_voisins, int nb_iter_recherche_local);
// ----------------------------------------------
void generer_fichier_gantt(t_probleme P, t_solution S, string nom_fichier);
void generer_fichier_power(t_solution S,string nom_fichier);
#endif
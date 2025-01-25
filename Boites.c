#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h> // Pour gérer les signaux

#define generations 100

// Structure pour une solution
typedef struct solution {
    int info_obj[1000]; // Emplacement des objets dans les boîtes
    int ind_valid; // Validité de la solution
} solu_t;

// Structure pour une population
typedef struct population {
    solu_t **solu; // Tableau de pointeurs vers des solutions
    int taille_population; // Taille de la population
} pop_t;

solu_t meilleure_solution_globale;

//Déclaration des foncion 
void afficher_solution(solu_t , int );
void gerer_arret(int );
int max(solu_t , int );
int ind_validite(solu_t , int *, int , int ); 
void organiser_boites(solu_t *, int );
solu_t mutation(solu_t , int );
solu_t croisement(solu_t , solu_t , int );
solu_t selectionner(pop_t , int , int , int *);
pop_t generer_pop(int , int , int *, int );
void liberer_population(pop_t );
solu_t meilleure_solution(pop_t , int , int *, int);
solu_t Algo_genetique(int , int , int , int *);
void lire_fichier(const char *, int *, int *, int **);


// Afficher une solution
void afficher_solution(solu_t solution, int nb_objets) {
    printf("Solution :\n");
    for (int i = 0; i < nb_objets; i++) {
        printf("Objet %d -> Boîte %d\n", i + 1, solution.info_obj[i]);
    }
    printf("Validité : %d\n", solution.ind_valid);
}

// Fonction pour gérer l'arrêt avec Ctrl+C
void gerer_arret(int nb_objets) {
    printf("\nInterruption détectée (Ctrl+C) !\n");
    printf("Meilleure solution globale trouvée :\n");
    printf("La meilleure solution est la precedente\n");
    exit(0); // Quitter proprement le programme
}

// Trouver le maximum dans une solution
int max(solu_t nb, int nb_objets) {
    int maximum = nb.info_obj[0];
    for (int i = 1; i < nb_objets; i++) {
        if (nb.info_obj[i] > maximum) {
            maximum = nb.info_obj[i];
        }
    }
    return maximum;
}

// Calculer la validité d'une solution
int ind_validite(solu_t individu, int *taille_objet, int nb_objets, int taille_boite) {
    int nb_boites = max(individu, nb_objets) + 1; // Nombre de boîtes utilisées
    int boite_utilise[nb_boites];
    memset(boite_utilise, 0, sizeof(boite_utilise));

    for (int i = 0; i < nb_objets; i++) {
        int boite = individu.info_obj[i];
        if (boite < 0 || boite >= nb_boites) {
            return 100000; // Indice invalide
        }
        boite_utilise[boite] += taille_objet[i];
        if (boite_utilise[boite] > taille_boite) {
            return 100000; // Solution invalide
        }
    }

    int nb_boites_utilisees = 0;
    for (int i = 0; i < nb_boites; i++) {
        if (boite_utilise[i] > 0) {
            nb_boites_utilisees++;
        }
    }
    return nb_boites_utilisees; // Retourne le nombre de boîtes utilisées
}

// Organiser les boîtes pour une solution
void organiser_boites(solu_t *solution, int nb_objets) {
    int plan[nb_objets];
    for(int i=0; i<nb_objets; i++) {
      plan[i]=0;
    }
    int prochaine_boite = 1;

    for (int i = 0; i < nb_objets; i++) {
        int boite = solution->info_obj[i];
        if (plan[boite] == 0) {
            plan[boite] = prochaine_boite++;
        }
        solution->info_obj[i] = plan[boite];
    }
}


// Fonction de mutation
solu_t mutation(solu_t individu, int nb_objets) {
    int id1 = rand() % nb_objets;
    int id2 = rand() % nb_objets;
    int temp = individu.info_obj[id1];
    individu.info_obj[id1] = individu.info_obj[id2];
    individu.info_obj[id2] = temp;
    return individu;
}

// Fonction de croisement
solu_t croisement(solu_t parent1, solu_t parent2, int nb_objets) {
    solu_t enfant;
    int id1 = rand() % nb_objets;
    int i;

    for (i = 0; i <= id1; i++) {
        enfant.info_obj[i] = parent1.info_obj[i];
    }

    for (; i < nb_objets; i++) {
        enfant.info_obj[i] = parent2.info_obj[i];
    }

    return enfant;
}

// La fonction de sélection
solu_t selectionner(pop_t pop, int nb_objets, int taille_boite, int *taille_objet) {
    int id1 = rand() % pop.taille_population;
    int id2 = rand() % pop.taille_population;
    if (ind_validite(*pop.solu[id1], taille_objet, nb_objets, taille_boite) > ind_validite(*pop.solu[id2], taille_objet, nb_objets, taille_boite))
        return *pop.solu[id2];
    else
        return *pop.solu[id1];
}

// Générer une population initiale valide
pop_t generer_pop(int taille_population, int nb_objets, int *taille_objet, int taille_boite) {
    pop_t population;
    population.taille_population = taille_population;
    population.solu = (solu_t **)malloc(taille_population * sizeof(solu_t *));

    srand(time(NULL));
    for (int i = 0; i < taille_population; i++) {
        population.solu[i] = (solu_t *)malloc(sizeof(solu_t));
        do {
            for (int j = 0; j < nb_objets; j++) {
                population.solu[i]->info_obj[j] = rand() % nb_objets;
            }
        } while (ind_validite(*population.solu[i], taille_objet, nb_objets, taille_boite) == 100000);

        population.solu[i]->ind_valid = ind_validite(*population.solu[i], taille_objet, nb_objets, taille_boite);
    }

    return population;
}

// Libérer la mémoire d'une population
void liberer_population(pop_t pop) {
    for (int i = 0; i < pop.taille_population; i++) {
        free(pop.solu[i]);
    }
    free(pop.solu);
}

// Trouver la meilleure solution dans une population
solu_t meilleure_solution(pop_t pop, int nb_objets, int *taille_objet, int taille_boite) {
    solu_t meilleure = *pop.solu[0];
    int meilleure_validite = ind_validite(meilleure, taille_objet, nb_objets, taille_boite);

    for (int i = 1; i < pop.taille_population; i++) {
        int validite_courante = ind_validite(*pop.solu[i], taille_objet, nb_objets, taille_boite);
        if (validite_courante < meilleure_validite) {
            meilleure = *pop.solu[i];
            meilleure_validite = validite_courante;
        }
    }

    organiser_boites(&meilleure, nb_objets);
    return meilleure;
}

// Algorithme génétique
solu_t Algo_genetique(int taille_population, int nb_objets, int taille_boite, int *taille_objet) {
    pop_t population = generer_pop(taille_population, nb_objets, taille_objet, taille_boite);

    for (int gen = 0; gen < generations; gen++) {
        pop_t nouvelle_population;
        nouvelle_population.taille_population = population.taille_population;
        nouvelle_population.solu = (solu_t **)malloc(nouvelle_population.taille_population * sizeof(solu_t *));

        for (int i = 0; i < nouvelle_population.taille_population; i++) {
            nouvelle_population.solu[i] = (solu_t *)malloc(sizeof(solu_t));
        }

        for (int i = 0; i < taille_population; i++) {
            solu_t parent1 = selectionner(population, nb_objets, taille_boite, taille_objet);
            solu_t parent2 = selectionner(population, nb_objets, taille_boite, taille_objet);
            solu_t enfant = croisement(parent1, parent2, nb_objets);
            if (rand() % 100 < 20) {
                enfant = mutation(enfant, nb_objets);
            }
            enfant.ind_valid = ind_validite(enfant, taille_objet, nb_objets, taille_boite);
            *nouvelle_population.solu[i] = enfant;
        }

        liberer_population(population);
        population = nouvelle_population;
    }

    solu_t meilleure = meilleure_solution(population, nb_objets, taille_objet, taille_boite);
    liberer_population(population);
    return meilleure;
}
// Fonction pour lire les données du fichier
void lire_fichier(const char *nom_fichier, int *taille_boite, int *nb_objets, int **tailles_objets) {
    FILE *fichier = fopen("projet_param.txt", "r");
    if (!fichier) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    char ligne[256];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (ligne[0] == 'B') {
            // Lire la taille des boîtesl
            sscanf(ligne, "B: %d", taille_boite);
        } else if (ligne[0] == 'n') {
            // Lire le nombre d'objets
            sscanf(ligne, "n: %d", nb_objets);
        } else if (ligne[0] == 'T') {
            // Lire les tailles des objets
            *tailles_objets = (int *)malloc((*nb_objets) * sizeof(int));
            if (*tailles_objets == NULL) {
                perror("Erreur d'allocation mémoire pour les tailles d'objets");
                fclose(fichier);
                exit(EXIT_FAILURE);
            }

            char *token = strtok(ligne + 2, " ");
            for (int i = 0; i < *nb_objets; i++) {
                if (token != NULL) {
                    (*tailles_objets)[i] = atoi(token);
                    token = strtok(NULL, " ");
                }
            }
        }
    }

    fclose(fichier);
}
// Programme principal
int main() {
    

    int nb_objets ;
    int taille_boite;
    int *taille_objet;
    int taille_population = 100 ;
   lire_fichier("projet_param", &taille_boite, &nb_objets, &taille_objet);
  signal(SIGINT, gerer_arret);
    meilleure_solution_globale.ind_valid = 100000;

    for (int gen = 0; ; gen++) {
        solu_t meilleure = Algo_genetique(taille_population, nb_objets, taille_boite, taille_objet);

        if (meilleure.ind_valid < meilleure_solution_globale.ind_valid) {
            meilleure_solution_globale = meilleure;
             printf("Meilleure solution après génération %d :\n", gen + 1);
            afficher_solution(meilleure_solution_globale, nb_objets);
            printf("\n");
        }
    }
    return 0;
}



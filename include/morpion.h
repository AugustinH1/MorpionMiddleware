#include "../libINET/data.h"
#include <pthread.h>
#include <semaphore.h>

#define PORT_SVC 5000
#define IP_LO0 "127.0.0.1"
#define MAX_CLIENT 10


typedef struct plateau {
    char tab[3][3];
    int joueurGagnant; //0: pas encore de gagnant, 1: joueur 1, 2: joueur 2, 3: égalité
}plateau;

typedef struct coord {
    int ligne;
    int colonne;
}coord;

typedef struct {
    int id;//id du joueur
    char ip[16];
    u_int16_t port;
    int waitJoueur;
}joueur_t;

typedef struct {
    int id;// id de la requette
    char message[MAX_BUFFER];
    joueur_t joueur;
}requete_t;


void printPlateau(plateau *plateau);
void initPlateau(plateau *plateau);
int evalPlateau(plateau *plateau, char figure);
int verifCoord(plateau plateau, coord *coord);

void deserialize_requete(char *buffer, requete_t *r);
void serialize_requete(requete_t *r, char *buffer);
void serialize_tab_requte(requete_t *tab, char *buffer);
void deserialize_tab_requete(char *buffer, requete_t *tab);
void serialize_plateau(plateau *plateau, char *buffer);
void deserialize_plateau(char *buffer, plateau *plateau);
void serialize_coord(coord *coord, char *buffer);
void deserialize_coord(char *buffer, coord *coord);
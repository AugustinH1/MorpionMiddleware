#include "../libINET/data.h"
#define PORT_SVC 5000
#define IP_LO0 "127.0.0.1"


typedef struct plateau {
    char tab[3][3];
}plateau;

typedef struct coord {
    int ligne;
    int colonne;
}coord;

typedef struct {
    int id;// id de la requette
    int joueur;
    //ip et port du joueur en system
    struct in_addr ip;   
    short port;
    int waitJoueur; //1 si en attente, 0 si ok, 2 si non initialiser
}requete_t;


void printPlateau(plateau *plateau);
void initPlateau(plateau *plateau);
int evalPlateau(plateau *plateau);
int verifCoord(plateau plateau, coord *coord);

void deserialize_requete(char *buffer, requete_t *r);
void serialize_requete(requete_t *r, char *buffer);
void serialize_tab_requte(requete_t *tab, char *buffer);
void serialize_tab_requte(requete_t *tab, char *buffer);
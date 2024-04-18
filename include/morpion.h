#include "../libINET/data.h"
#include <pthread.h>
#include <semaphore.h>

#define PORT_SVC 5000
#define IP_LO0 "127.0.0.1"
#define MAX_CLIENT 10


/**
 * \struct plateau
 * \brief Structure du plateau de jeu
*/
typedef struct plateau {
    char tab[3][3];
    int joueurGagnant; //0: pas encore de gagnant, 1: joueur 1, 2: joueur 2, 3: égalité
}plateau;

/**
 * \struct coord
 * \brief Structure des coordonnées
*/
typedef struct coord {
    int ligne;
    int colonne;
}coord;

/**
 * \struct joueur_t
 * \brief Structure du joueur
*/
typedef struct {
    int id;//id du joueur
    char ip[16];
    u_int16_t port;
    int waitJoueur;
}joueur_t;

/**
 * \struct requete_t
 * \brief Structure de la requête
*/
typedef struct {
    int id;// id de la requette
    char message[MAX_BUFFER];
    joueur_t joueur;
}requete_t;

/**
 * \fn void printPlateau(plateau *plateau)
 * \brief Affiche le plateau de jeu
 * \param plateau *plateau
 * \return void
*/
void printPlateau(plateau *plateau);

/**
 * \fn void initPlateau(plateau *plateau)
 * \brief Initialise le plateau de jeu
 * \param plateau *plateau
 * \return void
*/
void initPlateau(plateau *plateau);

/**
 * \fn int jouer(plateau *plateau, char figure)
 * \brief Joue un coup
 * \param plateau *plateau
 * \param char figure: figure a evaluer (X ou O)
 * \return int
*/
int evalPlateau(plateau *plateau, char figure);

/**
 * \fn int verifCoord(plateau plateau, coord *coord)
 * \brief Vérifie si les coordonnées sont valides
 * \param plateau plateau
 * \param coord *coord: coordonnées à vérifier
 * \return int
*/
int verifCoord(plateau plateau, coord *coord);


/**
 * \fn void serialize_requete(requete_t *r, char *buffer)
 * \brief Serialize une requête
 * \param requete_t *r: requête à sérialiser
 * \param char *buffer: buffer de destination
 * \return void
*/
void deserialize_requete(char *buffer, requete_t *r);
/**
 * \fn void deserialize_requete(char *buffer, requete_t *r)
 * \brief Deserialize une requête
 * \param char *buffer: buffer à désérialiser
 * \param requete_t *r: requête de destination
 * \return void
*/
void serialize_requete(requete_t *r, char *buffer);

/**
 * \fn void serialize_tab_requte(requete_t *tab, char *buffer)
 * \brief Serialize un tableau de requêtes
 * \param requete_t *tab: tableau de requêtes à sérialiser
 * \param char *buffer: buffer de destination
 * \return void
*/
void serialize_tab_requte(requete_t *tab, char *buffer);
/**
 * \fn void deserialize_tab_requete(char *buffer, requete_t *tab)
 * \brief Deserialize un tableau de requêtes
 * \param char *buffer: buffer à désérialiser
 * \param requete_t *tab: tableau de destination
 * \return void
*/
void deserialize_tab_requete(char *buffer, requete_t *tab);

/**
 * \fn void serialize_plateau(plateau *plateau, char *buffer)
 * \brief Serialize un plateau
 * \param plateau *plateau: plateau à sérialiser
 * \param char *buffer: buffer de destination
 * \return void
*/
void serialize_plateau(plateau *plateau, char *buffer);
/**
 * \fn void deserialize_plateau(char *buffer, plateau *plateau)
 * \brief Deserialize un plateau
 * \param char *buffer: buffer à désérialiser
 * \param plateau *plateau: plateau de destination
 * \return void
*/
void deserialize_plateau(char *buffer, plateau *plateau);

/**
 * \fn void serialize_coord(coord *coord, char *buffer)
 * \brief Serialize des coordonnées
 * \param coord *coord: coordonnées à sérialiser
 * \param char *buffer: buffer de destination
 * \return void
*/
void serialize_coord(coord *coord, char *buffer);
/**
 * \fn void deserialize_coord(char *buffer, coord *coord)
 * \brief Deserialize des coordonnées
 * \param char *buffer: buffer à désérialiser
 * \param coord *coord: coordonnées de destination
 * \return void
*/
void deserialize_coord(char *buffer, coord *coord);
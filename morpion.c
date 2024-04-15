#include "morpion.h"

void initPlateau(plateau *plateau) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            plateau->tab[i][j] = ' ';
        }
    }
}

void printPlateau(plateau *plateau) {
    printf("  0 | 1 | 2\n");
    printf("  ---------\n");
    for (int i = 0; i < 3; i++) {
        printf("%d |", i);
        for (int j = 0; j < 3; j++) {
            printf(" %c |", plateau->tab[i][j]);
        }
        printf("\n  ---------\n");
    }
}

int evalPlateau(plateau *plateau) {
    //je vérifie les lignes
    for (int i = 0; i < 3; i++)
        if (plateau->tab[i][0] == plateau->tab[i][1] && plateau->tab[i][1] == plateau->tab[i][2] && plateau->tab[i][0] != ' ')
            return 1;


    //je vérifie les colonnes
    for (int i = 0; i < 3; i++)
        if (plateau->tab[0][i] == plateau->tab[1][i] && plateau->tab[1][i] == plateau->tab[2][i] && plateau->tab[0][i] != ' ')
            return 1;


    //je vérifie les diagonales
    if (plateau->tab[0][0] == plateau->tab[1][1] && plateau->tab[1][1] == plateau->tab[2][2] && plateau->tab[0][0] != ' ')
        return 1;

    if (plateau->tab[0][2] == plateau->tab[1][1] && plateau->tab[1][1] == plateau->tab[2][0] && plateau->tab[0][2] != ' ')
        return 1;

    return 0;
}

int verifCoord(plateau plateau, coord *coord) {
    //verifier si les coordonnées sont dans le plateau
    if (coord->ligne < 0 || coord->ligne > 2 || coord->colonne < 0 || coord->colonne > 2)
        return 0;
    
    //verifier si la case est vide
    if (plateau.tab[coord->ligne][coord->colonne] != ' ')
        return 0;
    

    return 1;

}


void serialize_requete(requete_t *r, char *buffer){
    sprintf(buffer, "%d:%d:%s:%d:%d",r->id, r->joueur.id, r->joueur.ip, r->joueur.port, r->joueur.waitJoueur);
}

void deserialize_requete(char *buffer, requete_t *r) {
    sscanf(buffer, "%d:%d:%[^:]:%d:%d",&r->id, &r->joueur.id, r->joueur.ip, &r->joueur.port, &r->joueur.waitJoueur);
}

void serialize_tab_requte(requete_t *tab, char *buffer){
    //Clean buffer
    memset(buffer, 0, MAX_BUFFER);
    for (int i = 0; i < MAX_CLIENT; i++) {
        buffer_t tmp;
        serialize_requete(&tab[i], tmp);
        strcat(buffer, tmp);
        strcat(buffer, "_");
    }
}

void deserialize_tab_requete(char *buffer, requete_t *tab){
    char *token = strtok(buffer, "_");
    int i = 0;
    while (token != NULL) { 
        deserialize_requete(token, &tab[i]);
        token = strtok(NULL, "_");
        i++;
    }
}
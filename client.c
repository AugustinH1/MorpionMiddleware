#include "include/morpion.h"

socket_t sockEcouteSrvjeu;
sem_t semJeu;

/**
 * \fn void *dialSvrEnr()
 * \brief Fonction de dialogue avec le serveur d'enregistrement
 * \param void
*/
void *jouerClt2Srv(void *arg);

/**
 * \fn void *srvJeu()
 * \brief Fonction de dialogue avec le serveur de jeu
 * \param void
*/
void *jouerSvr2Clt(void *arg);

/**
 * \fn void *srvJeu()
 * \brief Fonction de dialogue avec le serveur de jeu
 * \param void
*/
void *srvJeu();

/**
 * \fn void *dialSvrEnr()
 * \brief Fonction de dialogue avec le serveur d'enregistrement
 * \param void
*/
void *dialSvrEnr();

int main() {

    sem_init(&semJeu, 0, 0);


    //lancement serveur 
    pthread_t threadJeu;
    pthread_create(&threadJeu, NULL, srvJeu, NULL);

    sem_wait(&semJeu);

    //lancement d'un client
    pthread_t threadClient;
    pthread_create(&threadClient, NULL, dialSvrEnr, NULL);


    //attendre que les threads se terminent
    pthread_join(threadJeu, NULL);
    pthread_join(threadClient, NULL);
    return 0;
}

void *dialSvrEnr(){
    socket_t sockDialogue;

    sockDialogue = connecterClt2Srv(IP_LO0, PORT_SVC);
    
    //requete d'enregistrement
    requete_t requete;
    requete.id = 200;
    strcpy(requete.joueur.ip, inet_ntoa(sockEcouteSrvjeu.addrLoc.sin_addr));
    requete.joueur.port = ntohs(sockEcouteSrvjeu.addrLoc.sin_port);

    envoyer(&sockDialogue, &requete, (pFct)serialize_requete);
    recevoir(&sockDialogue, &requete, (pFct)deserialize_requete);



    //recupérer la liste des partie en cours
    requete.id = 203;
    envoyer(&sockDialogue, &requete, (pFct)serialize_requete);

    requete_t joueurs[MAX_CLIENT];
    recevoir(&sockDialogue, &joueurs, (pFct)deserialize_tab_requete);



    //chercher un joueur en attente qui n'est pas nous
    for(int i = 0; i < MAX_CLIENT; i++){
        if((joueurs[i].joueur.waitJoueur == 1) 
                && (strcmp(joueurs[i].joueur.ip, inet_ntoa(sockEcouteSrvjeu.addrLoc.sin_addr)) == 0)
                && (joueurs[i].joueur.port == ntohs(sockEcouteSrvjeu.addrLoc.sin_port)) == 0){
            
            //printf("Joueur [%d]:[%s]:[%d] status d'attente [%d]\n", i, joueurs[i].joueur.ip, joueurs[i].joueur.port, joueurs[i].joueur.waitJoueur);


            //dire au serveur que le joueur est pris
            requete.id = 204;
            sprintf(requete.message, "%d", i);
            envoyer(&sockDialogue, &requete, (pFct)serialize_requete);
            

            //on se connecte a celui qu'on trouve
            socket_t sockDialogueJeu;
            sockDialogueJeu = connecterClt2Srv(joueurs[i].joueur.ip, joueurs[i].joueur.port);

            //crée un thread de jeu
            pthread_t thread;
            pthread_create(&thread, NULL, jouerClt2Srv,(void *)&sockDialogueJeu);
    
            
            pthread_exit(NULL);
            
        }
    }

    pthread_exit(NULL);
}



void *srvJeu(){
    socket_t sockDialogue;
    pthread_t thread;

    sockEcouteSrvjeu = creerSocketEcoute("0.0.0.0", 0);
    sem_post(&semJeu);

    while (1){
        sockDialogue = accepterClt(sockEcouteSrvjeu);
        pthread_create(&thread, NULL, jouerSvr2Clt,(void *)&sockDialogue);
    }

    close (sockDialogue.fd);
    close(sockEcouteSrvjeu.fd);

    pthread_exit(NULL);

}




void *jouerSvr2Clt(void *arg){
    socket_t sockDialogue = *(socket_t *)arg;

    printf("jeu Srv\n");

    plateau plateau;
    initPlateau(&plateau);

    coord coord;

    while(1){
        printPlateau(&plateau);
        //demander coordonnées au joueur
        printf("Entrez les coordonnées x et y de votre coup (entre 0 et 2) : ");
        scanf("%d %d", &coord.ligne, &coord.colonne);

        //vérifier les coordonnées
        while (verifCoord(plateau, &coord) == 0){
            printf("Coordonnées invalides, veuillez réessayer : ");
            scanf("%d %d", &coord.ligne, &coord.colonne);
        }

        //mettre à jour le plateau
        plateau.tab[coord.ligne][coord.colonne] = 'X';
        printPlateau(&plateau);

        if (evalPlateau(&plateau, 'X') == 1){
            plateau.joueurGagnant = 1;
            envoyer(&sockDialogue, &plateau, (pFct)serialize_plateau);
            printf("Vous avez gagné\n");
            break;
        }
        if (evalPlateau(&plateau, 'X') == 3){
            plateau.joueurGagnant = 3;
            envoyer(&sockDialogue, &plateau, (pFct)serialize_plateau);
            printf("Egalité\n");
            break;
        }

        //envoyer le plateau au client
        envoyer(&sockDialogue, &plateau, (pFct)serialize_plateau);

        //recevoir les coordonnées du client
        recevoir(&sockDialogue, &coord, (pFct)deserialize_coord);

        //mettre à jour le plateau
        plateau.tab[coord.ligne][coord.colonne] = 'O';

        if (evalPlateau(&plateau, 'O') == 1){
            plateau.joueurGagnant = 2;
            printf("Vous avez perdu\n");
            break;
        }
        if(evalPlateau(&plateau, 'O') == 3){
            plateau.joueurGagnant = 3;
            printf("Egalité\n");
            break;
        }

    }
    

    pthread_exit(NULL);
}

void *jouerClt2Srv(void *arg){
    socket_t sockDialogue = *(socket_t *)arg;

    printf("jeu Clt\n");

    plateau plateau;

    while(1){
        //recevoir le plateau
        recevoir(&sockDialogue, &plateau, (pFct)deserialize_plateau);
        printPlateau(&plateau);

        if (plateau.joueurGagnant == 1){
            printf("Vous avez perdu\n");
            break;
        }
        if (plateau.joueurGagnant == 3){
            printf("Egalité\n");
            break;
        }
        

        coord coord;
        //demander coordonnées au joueur
        printf("Entrez les coordonnées x et y de votre coup (entre 0 et 2) : ");
        scanf("%d %d", &coord.ligne, &coord.colonne);

        //vérifier les coordonnées
        while (verifCoord(plateau, &coord) == 0){
            printf("Coordonnées invalides, veuillez réessayer : ");
            scanf("%d %d", &coord.ligne, &coord.colonne);
        }

        //mettre à jour le plateau
        plateau.tab[coord.ligne][coord.colonne] = 'O';
        printPlateau(&plateau);

        //envoyer les coordonnées au serveur
        envoyer(&sockDialogue, &coord, (pFct)serialize_coord);

        if(evalPlateau(&plateau, 'O') == 1){
            plateau.joueurGagnant = 2;
            printf("Vous avez gagné\n");
            break;
        }
        if(evalPlateau(&plateau, 'O') == 3){
            plateau.joueurGagnant = 3;
            printf("Egalité\n");
            break;
        }
    }


    
    
    pthread_exit(NULL);
}

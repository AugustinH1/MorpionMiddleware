//Morpion en réseau avec 2 clients et un serveur
//ici le client

#include "morpion.h"
#include <pthread.h>
#include <semaphore.h>

socket_t sockEcouteSrvjeu;
sem_t semJeu;

void *jouerClt2Srv(void *arg){
    socket_t sockDialogue = *(socket_t *)arg;

    //dialogue de jeu en appelant un joueur
    printf("jeu en cours\n");
    //le jeu est la
    pthread_exit(NULL);
}


void *jouerSvr2Clt(void *arg){
    socket_t sockDialogue = *(socket_t *)arg;

    //dialogue de jeu en reponse a un joueur
    printf("jeu en cours\n");
    //le jeu est la
    

    pthread_exit(NULL);
}

void *srvJeu(void *arg){
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

void *dialSvrEnr(void *arg){
    socket_t sockDialogue;

    sockDialogue = connecterClt2Srv(IP_LO0, PORT_SVC);

    //requete d'enregistrement
    requete_t requete;
    requete.id = 200;
    requete.ip = sockDialogue.addrLoc.sin_addr;
    requete.port = sockDialogue.addrLoc.sin_port;

    envoyer(&sockDialogue, &requete, serialize_requete);


    while(1){
        printf("Que voulez vous faire?\n");
        printf("1: se connecter\n");
        printf("2: lister les parties\n");
        printf("5: quitter\n");


        int choix;
        scanf("%d", &choix);

        switch (choix)
        {
            case 1:
                requete.id = 200;
                requete.waitJoueur = 0;
                envoyer(&sockDialogue, &requete, serialize_requete);


                socket_t sockDialogue;
                pthread_t thread;
                pthread_create(&thread, NULL, jouerClt2Srv,(void *)&sockDialogue);

                pthread_exit(NULL);
                break;
            case 2:
                requete.id = 203;
                envoyer(&sockDialogue, &requete, serialize_requete);

                requete_t joueurs[MAX_BUFFER];
                recevoir(&sockDialogue, &joueurs, serialize_tab_requte);
                printf("Liste des connectés recu\n");
            
                for(int i = 0; i < MAX_BUFFER; i++){
                    if(joueurs[i].waitJoueur == 1){
                        printf("Joueur %d: %s:%d\n", i, inet_ntoa(joueurs[i].ip), joueurs[i].port);
                    }
                }
                
                break;
            
            case 5:
                requete.id = 204;
                pthread_exit(NULL);
                break;
            default:
                break;
        }
    }





    pthread_exit(NULL);
}

int main() {

    sem_init(&semJeu, 0, 0);


    //lancement serveur 
    pthread_t threadJeu;
    pthread_create(&threadJeu, NULL, srvJeu, NULL);

    sem_wait(&semJeu);

    //lancement d'un client
    pthread_t threadClient;
    pthread_create(&threadClient, NULL, dialSvrEnr, NULL);
    
    //dialogue avec l'utilisateur humain


    //attendre que les threads se terminent
    pthread_join(threadJeu, NULL);
    pthread_join(threadClient, NULL);
    return 0;
}
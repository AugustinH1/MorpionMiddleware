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

    PAUSE("CONNEXION");
    sockDialogue = connecterClt2Srv(IP_LO0, PORT_SVC);
    
    //requete d'enregistrement
    requete_t requete;
    requete.id = 200;
    strcpy(requete.joueur.ip, inet_ntoa(sockEcouteSrvjeu.addrLoc.sin_addr));
    requete.joueur.port = ntohs(sockEcouteSrvjeu.addrLoc.sin_port);

    printf("Requete envoye: [%d] [%s] [%d]\n", requete.id, requete.joueur.ip, requete.joueur.port);
    envoyer(&sockDialogue, &requete, (pFct)serialize_requete);


    PAUSE("LISTE DES JOUEURS")
    //liste les partie en cours
    requete.id = 203;
    printf("Requete envoye: [%d]\n", requete.id);
    envoyer(&sockDialogue, &requete, (pFct)serialize_requete);

    requete_t joueurs[MAX_CLIENT];
    recevoir(&sockDialogue, &joueurs, (pFct)deserialize_tab_requete);


    printf("Liste des connectés recu\n");
    
    for(int i = 0; i < MAX_CLIENT; i++){
        //verifier qu'il soit en attente et pas luis meme
        if((joueurs[i].joueur.waitJoueur == 1) 
            && (strcmp(joueurs[i].joueur.ip, inet_ntoa(sockEcouteSrvjeu.addrLoc.sin_addr)) == 0)
            && (joueurs[i].joueur.port == ntohs(sockEcouteSrvjeu.addrLoc.sin_port)) == 0){
            printf("Joueur [%d]:[%s]:[%d] status d'attente [%d]\n", i, joueurs[i].joueur.ip, joueurs[i].joueur.port, joueurs[i].joueur.waitJoueur);


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
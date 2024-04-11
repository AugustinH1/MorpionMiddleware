//Morpion en réseau avec 2 clients et un serveur
//ici le client

#include "morpion.h"
#include <pthread.h>
#include <semaphore.h>

socket_t sockEcouteSrvjeu;
sem_t semJeu;

void *jouerClt2Srv(void *arg){
    //dialogue de jeu en appelant un joueur
    pthread_exit(NULL);
}


void *jouerSvr2Clt(void *arg){
    //dialogue de jeu en reponse a un joueur
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

        
        close (sockDialogue.fd);

    }
    close(sockEcouteSrvjeu.fd);

    pthread_exit(NULL);

}

void *dialSvrEnr(void *arg){
    socket_t sockDialogue;

    sockDialogue = connecterClt2Srv(IP_LO0, PORT_SVC);

    //requete d'enregistrement
    requete_t requete;
    requete.id = 100;
    strcpy(requete.ip, inet_ntoa(sockDialogue.addrLoc.sin_addr));
    requete.port = ntohs(sockDialogue.addrLoc.sin_port);

    envoyer(&sockDialogue, &requete, serialize_requete);

    // dialogue effectif avec dialClt et utilisateur final
    // ex requete: list des connecter/lancer un jeu
    //quand on lance un jeu on crée un thread qui va appeler un srv de jeu
    //je me connecte au servuer de jeu
    //pthread_create(&thread, NULL, jouerClt2Srv,(void *)&sockDialogue);



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


    
    return 0;
}
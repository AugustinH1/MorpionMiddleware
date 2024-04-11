//Morpion en réseau avec 2 clients et un serveur
// watch -d -n0.1 'netstat --inet -anp | grep 5000'
//ici le serveur



#include "morpion.h"
#include <pthread.h>
#include <semaphore.h>



//tableau global des joueurs
requete_t joueurs[100];
sem_t semJeu;
int nbClt = 0;


void *dialClt(void *arg){
    socket_t sockDialogue = *(socket_t *)arg;
    // je recoie la première requête d'enregistrement
    requete_t requete;
    recevoir(&sockDialogue, &requete, deserialize_requete);

    requete.joueur = nbClt;
    joueurs[nbClt] = requete;
    sem_post(&semJeu);

    // dialogue effectif avec dialSrvEnr
    //demander ip de l'autre client: table des clients

    pthread_exit(NULL);
}


int main() {
    socket_t sockDialogue;
    pthread_t thread;

    sem_init(&semJeu, 0, 0);

    //je crée ma socket d'écoute
    socket_t sockEcoute;
    sockEcoute = creerSocketEcoute(IP_LO0, PORT_SVC);
    

    while (1){
        
        sockDialogue = accepterClt(sockEcoute);
        pthread_create(&thread, NULL, dialClt,(void *)&sockDialogue);

        sem_wait(&semJeu);
        nbClt++;
        close (sockDialogue.fd);

    }

    close(sockEcoute.fd);
    return 0;
}
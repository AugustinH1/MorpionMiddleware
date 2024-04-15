//Morpion en réseau avec 2 clients et un serveur
// watch -d -n0.1 'netstat --inet -anp | grep 5000'
//ici le serveur



#include "morpion.h"
#include <pthread.h>
#include <semaphore.h>



//tableau global des joueurs
requete_t joueurs[MAX_BUFFER];
sem_t semJeu;
int nbClt = 0;


void *dialClt(void *arg){
    socket_t sockDialogue = *(socket_t *)arg;
    // je recoie la première requête d'enregistrement
    requete_t requete;
    recevoir(&sockDialogue, &requete, deserialize_requete);
    printf("Requete reçue: %d %s %d\n", 
    requete.id,
    inet_ntoa(requete.ip),
    ntohs(requete.port));

    requete.joueur = nbClt;
    requete.waitJoueur = 1;
    requete.joueur = nbClt;
    joueurs[nbClt] = requete;
    sem_post(&semJeu);

    printf("ici\n");
    while (1)
    {
        recevoir(&sockDialogue, &requete, deserialize_requete);
        printf("Requete reçue: %d\n", requete.id);

        switch (requete.id)
        {
            case 200:
                //connecter avec un autre client
                break;
            case 201:
                //disconect
                break;
            case 202:
                //create party
                break;
            case 203:
                //lister les parties
                for(int i = 0; i < MAX_BUFFER; i++){
                    if(joueurs[i].waitJoueur == 1){
                        printf("Joueur %d: %s:%d\n", i, inet_ntoa(joueurs[i].ip), joueurs[i].port);
                    }
                }
                envoyer(&sockDialogue, joueurs, serialize_tab_requte);
                break;
        
        
        default:
            //error
            break;
        }
        
    }
    

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
        
    }
    close (sockDialogue.fd);
    close(sockEcoute.fd);
    return 0;
}
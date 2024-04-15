//Morpion en réseau avec 2 clients et un serveur
// watch -d -n0.1 'netstat --inet -anp | grep 5000'
//ici le serveur



#include "morpion.h"
#include <pthread.h>
#include <semaphore.h>



//tableau global des joueurs
requete_t joueurs[MAX_CLIENT];
sem_t semJeu;
int nbClt = 0;


void *dialClt(void *arg){
    socket_t sockDialogue = *(socket_t *)arg;

    requete_t requete;


    while (1)
    {
        printf("En attente de requête\n");
        recevoir(&sockDialogue, &requete, (pFct)deserialize_requete);
        printf("Requete reçue: [%d] [%s] [%d]\n", requete.id, requete.joueur.ip, requete.joueur.port);

        switch (requete.id)
        {
            case 200:
                //connect
                requete.joueur.id = nbClt;
                requete.joueur.waitJoueur = 1;
                joueurs[nbClt] = requete;
                nbClt++;
                sem_post(&semJeu);
                break;
            case 201:
                //disconect
                break;
            case 202:
                //create party
                break;
            case 203:
                //lister les parties
                printf("Liste des connectés\n");
                envoyer(&sockDialogue, joueurs, (pFct)serialize_tab_requte);
                break;
            case 204:
                //join party
                joueurs[requete.joueur.id].joueur.waitJoueur = 0;
                int idToChange = atoi(requete.message);
                
                for (int i = 0; i < MAX_CLIENT; i++) {
                    if (joueurs[i].joueur.id == idToChange) {
                        printf("Joueur [%d] trouvé\n", idToChange);
                        joueurs[i].joueur.waitJoueur = 0;
                        break;
                    }
                }     
                           
                break;
            
            case 1000:
                //test requete
                printf("Requete test reçue\n");
                printf("Message: %s\n", requete.message);
                break;
        
        
        default:
            //error
            break;
        }
        
    }
    
    close(sockDialogue.fd);
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
        
    }
    close (sockDialogue.fd);
    close(sockEcoute.fd);
    return 0;
}
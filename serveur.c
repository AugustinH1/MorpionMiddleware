//watch -d -n0.1 'netstat --inet -anp | grep exe'

#include "include/morpion.h"



//tableau global des joueurs
requete_t joueurs[MAX_CLIENT];
sem_t semJeu;
int nbClt = 0;


/**
 * \fn void *dialClt(void *arg)
 * \brief Fonction de dialogue avec un client
 * \param void *arg: socket de dialogue
 * \return void
*/
void *dialClt(void *arg);

/**
 * \fn void requete(socket_t sockDialogue)
 * \brief Fonction de traitement des requêtes
 * \param socket_t sockDialogue: socket de dialogue
 * \return void
*/
void requete(socket_t sockDialogue);


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

void *dialClt(void *arg){
    socket_t sockDialogue = *(socket_t *)arg;

    requete(sockDialogue);
    
    close(sockDialogue.fd);
    pthread_exit(NULL);
}

void requete(socket_t sockDialogue){
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
                envoyer(&sockDialogue, &requete, (pFct)serialize_requete);
                nbClt++;
                sem_post(&semJeu);
                break;
            case 201:
                //disconect
                for (int i = 0; i < MAX_CLIENT; i++) {
                    if (joueurs[i].joueur.id == requete.joueur.id) {
                        joueurs[i].joueur.id = -1;
                        break;
                    }
                }
                break;
            case 203:
                //lister les parties
                envoyer(&sockDialogue, joueurs, (pFct)serialize_tab_requte);
                break;
            case 204:
                //join party
                joueurs[requete.joueur.id].joueur.waitJoueur = 0;
                int idToChange = atoi(requete.message);
                
                for (int i = 0; i < MAX_CLIENT; i++) {
                    if (joueurs[i].joueur.id == idToChange) {
                        joueurs[i].joueur.waitJoueur = 0;
                        break;
                    }
                }
                
                           
                break;
        default:
            //error
            break;
        }
        
    }
}

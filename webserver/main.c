#include <stdio.h>
#include <string.h>
#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#define BUFFER_SIZE 8000

void traitement_signal (int sig) { 
	printf("Signal %d reçu \n", sig);
	while(1){
		if(waitpid(-1, NULL, WNOHANG)<=0){
			break;
		}
	}
}

void initialiser_signaux (void) {
	if(signal(SIGPIPE,SIG_IGN)==SIG_ERR){
		perror ("signal");
	}
	struct sigaction sa;
	sa.sa_handler = traitement_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror ("sigaction(SIGCHLD)");
	}
}

int main (void) {
	/* Arnold Robbins in the LJ of February ’95 , describing RCS 
	if ( argc > 1 && strcmp ( argv [1] , " -advice " ) == 0) {
	printf ( " Don ’t Panic !\n " );
	return 42;
	}
	printf ( " Need an advice ?\n " );
	return 0;*/

	int socket_client;
	int socket_serveur = creer_serveur(8080);
	initialiser_signaux();
		
	while(1) {
		socket_client = accept(socket_serveur , NULL, NULL);
		if (socket_client == -1){
			perror ("accept");
			//traitement d ’ erreur
		}

		const char * message_bienvenue = "Bonjour, \nNous vous souhaitons la bienvenue sur notre serveur ! \nC'est un immense plaisir de vous voir ici. \nNous espérons que vous serez satisfait \net que tout se passera pour le mieux. \nEn attendant, \nnous vous souhaitons un agréable moment. \nSi vous rencontrez un quelconque problème \nn'hésitez pas à nous contacter.\nNous restons à votre entière disposition. \n";

		int pid = fork();
		if (pid == 0) {
			int i;
			for (i=0; i<1; i++) {	
				write(socket_client,message_bienvenue,strlen(message_bienvenue));
				sleep(1);
			}

			/*char buffer[BUFFER_SIZE];
			FILE* file = fopen("text.txt", "r");
			write(socket_client, fgets(buff, 255, file), 100000);
			fclose(file);
			
			while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
				write(socket_client, fgets(buffer, BUFFER_SIZE, file), BUFFER_SIZE);
			}
			fclose(file);*/
			
			while(1) {
				char saisie[1000] = "";
				if (read(socket_client, saisie, 1000) < 0) {
					exit(0);
				}
				write(socket_client, saisie, strlen(saisie));
			}

		} else {
			close(socket_client);		
		}
	}
}

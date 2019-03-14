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
#include <sys/stat.h>
#include "http_parse.h"

#define BUFFER_SIZE 1000

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

char * fgets_or_exit(char * buffer , int size , FILE * stream ){
	if (fgets(buffer, size, stream) == NULL){	
		exit(0);
	} 
	return buffer;
}

void skip_headers(FILE *client, char *saisie) {
	fgets_or_exit(saisie, BUFFER_SIZE, client);
	while (strcmp(&saisie[0], "\r\n") != 0 && strcmp(&saisie[0], "\n") != 0) {
		fgets_or_exit(saisie, BUFFER_SIZE, client);
	}
}

void send_status(FILE *client, int code, const char * reason_phrase) {
	fprintf(client, "HTTP/1.1 %d %s\r\n", code, reason_phrase);
}

void send_response(FILE *client, int code, const char *reason_phrase, const char *message_body) {
	send_status(client, code, reason_phrase);
	fprintf(client, "Content-Length : %li\r\n\r\n%s", strlen(message_body), message_body);
}

char *rewrite_target(char *target) {
	char * tmp  = strtok(target, "?");
	strcpy(target, tmp);
	int taille = strlen(target);
	if(target[taille-1] == '/' || taille==0){
		strcat(target, "index.html");
	}
	return target;
}

FILE *check_and_open(const char *target, const char *document_root) {
	char con[50];
	strcpy(con, document_root);
	strcat(con, target);
	struct stat s;
	stat(con, &s);
	if (S_ISREG(s.st_mode)) {
		return fopen(con, "r");
	}
	return NULL;
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
	http_request request;
		
	while(1) {
		socket_client = accept(socket_serveur , NULL, NULL);
		if (socket_client == -1){
			perror ("accept");
			//traitement d ’ erreur
		}

		char * message_bienvenue = "Bonjour, \nNous vous souhaitons la bienvenue sur notre serveur ! \nC'est un immense plaisir de vous voir ici. \nNous espérons que vous serez satisfait \net que tout se passera pour le mieux. \nEn attendant, \nnous vous souhaitons un agréable moment. \nSi vous rencontrez un quelconque problème \nn'hésitez pas à nous contacter.\nNous restons à votre entière disposition. \n";

		int pid = fork();
		FILE * f = fdopen(socket_client,"w+");
		if (pid == 0) {
			int lecture = 0;
			while(1) {
				if (f == NULL) {
					perror("Error opening file");
					exit(0);
				}

				char saisie[BUFFER_SIZE] = "";
				char nomServeur[BUFFER_SIZE] = "<Arnisserveur> ";

				fgets_or_exit(saisie, BUFFER_SIZE, f);

				strcat(nomServeur, saisie);
				char * ligne404 = "GET /inexistant HTTP/1.1\r\n";

				if (parse_http_request(saisie, &request) != 0) {
					skip_headers(f, saisie);
					fprintf(f, nomServeur);
					lecture = 1;
				} else if (strcmp(saisie, ligne404) == 0){
					send_response(f, 400, "Bad Request", "Bad request\r\n" );
				
				} else if (lecture == 0) {
					send_response(f, 400, "Bad Request", "Bad request \r\n");
				} else if (request.method == HTTP_UNSUPPORTED) {
					send_response(f, 405 , "Method Not Allowed", "Method Not Allowed\r\n");
				} else {
					send_response (f, 404, "Not Found", "Not Found\r\n " );
				}
				
				if (lecture == 1) {
					lecture = 0;
					send_response(f, 200, "OK", message_bienvenue);
				} else {
					fprintf(f, nomServeur);
				}
			}	
		} else {
			close(socket_client);		
		}
	}
}

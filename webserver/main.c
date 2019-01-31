#include <stdio.h>
#include <string.h>
#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <stdlib.h>

int main ( void)
{
	/* Arnold Robbins in the LJ of February ’95 , describing RCS 
	if ( argc > 1 && strcmp ( argv [1] , " -advice " ) == 0) {
	printf ( " Don ’t Panic !\n " );
	return 42;
	}
	printf ( " Need an advice ?\n " );
	return 0;*/

	int socket_client;
	int socket_serveur = creer_serveur(8080);

	while(1) {
		socket_client = accept(socket_serveur , NULL, NULL);
		if (socket_client == -1){
			perror ( "accept" );
			/* traitement d ’ erreur */
		}
		/* On peut maintenant dialoguer avec le client */
		//const char * message_bienvenue = " Bonjour , bienvenue sur mon serveur \n " ;
		char buff[255];
		FILE* file = fopen("text.txt", "r");
		//write(socket_client,message_bienvenue,strlen(message_bienvenue));
		write(socket_client, fgets(buff, 255, file), 100000);
		fclose(file);
	}
}

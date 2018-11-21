#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "send.h"
#include "client.h"


void do_connect2(int client_socket,struct sockaddr_in server_sock){
	int err = -1;
	while(err==-1)
		err=connect(client_socket,(struct sockaddr *)&server_sock,sizeof(struct sockaddr_in));
		printf(" Connecting to server... done!\n");
		fflush(stdout);

}

void send_file(char* ip_addr,int port,char* path){
	char* buffer3 = malloc(512*sizeof(char));
	int fin_fich = 0;
	int client_sock;
	struct sockaddr_in server;
	int* taille_fich = malloc(sizeof(int));
	*taille_fich = 0;
	int bit_sent = -1;

	//get the socket
	client_sock = do_socket();
	//init
	server = init_server_addr(ip_addr,port,server);
	//connect to remote socket
	do_connect2(client_sock,server);

	FILE * fich = fopen(path,"r");

	fseek(fich, 0,SEEK_END);
	*taille_fich = ftell(fich);
	rewind(fich);

	printf(" taille fichier: %d\n",*taille_fich);
	fflush(stdout);

	while(bit_sent == -1)
		bit_sent = send(client_sock, taille_fich, sizeof(int) , 0);

	if(fich != NULL )
	{
		while( fin_fich == 0) ///dernier octet du fichier
		{
			memset(buffer3,'\0',512);
			fgets(buffer3 , 512 , fich);

			send(client_sock, buffer3, 512 , 0);
			fin_fich = feof(fich);

		}
		fclose(fich);
		printf(" End\n");
		fflush(stdout);

	}else{
		printf(" No file to send\n");
		fflush(stdout);
	}
	free(taille_fich);
	free(buffer3);
	close(client_sock);
}

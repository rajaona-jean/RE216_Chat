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

char buffer[512];

//int do_socket(){
//	int s = socket(AF_INET,SOCK_STREAM,0);
//	if (s == -1){
//		fprintf(stdout , " [client]: Erreur création de socket 2\n");
//		fflush(stdout);
//		error("socket");close(s);exit(EXIT_FAILURE);
//	}
//	else{
//		fprintf(stdout , " [client]: Socket créée %d\n", s);
//		fflush(stdout);
//	}
//	return s;
//}
//
//struct sockaddr_in init_server_addr(char* ip_addr,int port,struct sockaddr_in server_sock){
//	memset(&server_sock,'\0',sizeof(struct sockaddr_in));
//	server_sock.sin_family = AF_INET;
//	server_sock.sin_port = htons(port);
//	server_sock.sin_addr.s_addr = inet_addr(ip_addr);
//	//inet_aton(ip_addr,&server_sock.sin_addr);
//	return server_sock;
//}
//
//void do_connect(int client_socket,struct sockaddr_in server_sock){
//	int err = -1;
//	while(err==-1)
//		err=connect(client_socket,(struct sockaddr *)&server_sock,sizeof(struct sockaddr_in));
//	if(err == -1){
//		error("connect");close(client_socket);exit(EXIT_FAILURE);
//	}
//	else{
//		printf(" Connecting to server... done!\n");
//		fflush(stdout);
//	}
//}

void send_file(char* ip_addr,int port,char* path){

	char dest[512];
	int fin_fich;
	int client_sock;
	struct sockaddr_in server;

	//get the socket
	client_sock = do_socket();
	//init
	server = init_server_addr(ip_addr,port,server);
	//connect to remote socket
	do_connect(client_sock,server);

	FILE * fich = fopen(path,"r");
	if(fich != NULL )
	{
		while( fin_fich == 0) ///dernier octet du fichier
		{
			send(client_sock, buffer, sizeof( buffer) , MSG_MORE);
			fwrite(buffer , 1 , strlen(buffer)*sizeof(char) , fich);
			fin_fich = feof(fich);

		}
		fclose(fich);

	}else{
		printf(" No file to send\n");
		fflush(stdout);
	}
}

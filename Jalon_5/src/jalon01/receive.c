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


#include "receive.h"
#include "client.h"


char buffer2[512];

void setsock(int socket_fd){
	int option = 1;
	int error = setsockopt(socket_fd,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option));
	if(error == -1){
		perror("setsockopt");
	}
}

int do_socket2(){
	int s = socket(AF_INET,SOCK_STREAM,0);
	setsock(s);
	if (s == -1){
		fprintf(stdout , " client_serveur: Erreur création de socket\n");
		fflush(stdout);
		error("socket");close(s); exit(EXIT_FAILURE);
	}
	else{
		fprintf(stdout , " socket crée\n");
		fflush(stdout);
	}
	return s;
}

int do_listen2(int server_sock,int connect_nb){
	int err;
	err = listen(server_sock,20);
	if(err == 0){
		printf(" [serveur]: LISTEN: OK\n");
		fflush(stdout);

	}
	else{
		printf(" listen\n");
		fflush(stdout);
		close(server_sock); error("listen");
	}
	return 0;
}

struct sockaddr_in init_sender(char* ip_addr,int port){
	struct sockaddr_in sin;
	memset(&sin,0,sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = ntohs(port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	return sin;
}

int do_bind2(int server_sock, struct sockaddr_in sin){
	int b = bind(server_sock,(struct sockaddr*)&sin, sizeof(sin));
	return b;
}


int do_accept2(int server_sock,struct sockaddr_in* c_sin){
	//	socklen_t* c_sin_size =(socklen_t*) sizeof(&c_sin);
	//	printf("%d",c_sin_size);
	//	fflush(stdout);
	int c_sin_size = sizeof(c_sin);
	int client_sock = accept(server_sock,(struct sockaddr*)&c_sin,&c_sin_size);
	printf(" connection done");
	fflush(stdout);
	return client_sock;
}


void receive_file(char*ip_addr,int port,char* path){

	struct sockaddr_in sin = init_sender(ip_addr,port);
	struct sockaddr_in c_sin;
	int fin_fich = 0; //quand s'erreter
	int s = do_socket2();

	//do_bind
	int b = do_bind2(s,sin);

	//do_listen
	do_listen2(s,1);

	//do_accept
	int client_sock = do_accept2(s,&c_sin);


	memset(buffer2,'\0',512);
	//char buffer;
	FILE * fich = fopen(path,"w+");
	if(fich == NULL )
	{
		while( fin_fich == 0) ///dernier octet du fichier
		{

			recv(s, buffer2, 512, 0);
			fputs(buffer2 , fich);
			printf(" %s",buffer2);
			fflush(stdout);


		}
		fclose(fich);

	}else{
		printf(" No file to send\n");
		fflush(stdout);
	}




}

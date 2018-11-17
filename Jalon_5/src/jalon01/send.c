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

#include "canal.h"
#include "user.h"
#include "server.h"
#include "send.h"

#define MAX_USER 4

char buffer[512];
int N = MAX_USER+1; //on compte le serveur

int do_socket2(){
	int s = socket(AF_INET,SOCK_STREAM,0);
	if (s == -1){
		fprintf(stdout , " client_serveur: Erreur création de socket\n");
		fflush(stdout);
		error("socket");close(s); exit(EXIT_FAILURE);
	}
	else{
		fprintf(stdout , " client_serveur: Socket créée %d\n", s);
		fflush(stdout);
	}
	return s;
}


struct sockaddr_in init_sender(int port){
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
	return client_sock;
}

char* do_write2(int client_sock,int server_sock){
	char* msg = buffer;
	int size_txt = strlen(msg);
	int s = send(client_sock,msg,size_txt,0);
	if(s!=-1){
		//		printf(" serveur: taille du message: %d \n server: taille du message envoyé: %d\n",size_txt, s);
		//		fflush(stdout);
	}
	else{
		close(server_sock);error("send");
	}
	return msg;
}

void send_file(char* path){

	char buffer2[512];
	struct sockaddr_in sin = init_sender(8080);
	struct sockaddr_in c_sin;
	int s = do_socket2();

	//do_bind
	int b = do_bind2(s,sin);

	//do_listen
	listen(s,20);


	//do_accept
	int sock_2 = do_accept2(s,&c_sin);
	int n=2;
	//int l = sizeof(path);

	struct pollfd fds[512] ;
	memset(fds,0,sizeof(fds));
	fds[0].fd = sock_2; //2eme user
	fds[0].events = POLLIN;
	fds[1].fd = STDIN_FILENO;
	fds[1].events = POLLIN;




	//char* NomFichier=malloc((l-1)*sizeof(char)); //fichier a envoyer

	int fin_buffer = 0; //quand s'erreter

	memset(buffer,'\0',512);
	//char buffer;
	int i = 0 , j = 1;
	/*for(i,j; path[j] != '\0';i++,j++){
		NomFichier[i] = path[j];
		printf("nom fichier %c\n",NomFichier[i]);
		fflush(stdout);
	}*/
	memset(buffer2,'\0',512);
	strcpy(buffer2,"I want to send you a file do you accept?");
	send(sock_2,buffer2,512,0);
	//send(client_sock,path,size_txt,0);
	for(;;){
		int p=poll(fds,n,-1);
		if (fds[0].revents == POLLIN){

			//printf("le nom du fichier est [%s]\n",NomFichier);
			FILE* fichier =fopen(path , "r");
			if ( fichier != NULL) // ce test échoue si le fichier n'est pas ouvert
			{
				while (fin_buffer == 0)
				{
					if( fread( buffer , sizeof(char) , 512 , fichier ) <= 0) //fin du buffer
					fin_buffer = 1;

					send(fds[0].fd, buffer, sizeof(buffer), 0);
					printf("%s\n" , buffer);
					memset(buffer,'\0',512);
				}

				printf("envoi %s fini \n", path);
				fclose(fichier);
			}
			//else
			printf("erreur ouverture fichier\n");

		}
	}

}

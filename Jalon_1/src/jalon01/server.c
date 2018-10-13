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

char buffer[512];

struct pollfd fds[512];


void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int do_socket(){
	int s = socket(AF_INET,SOCK_STREAM,0);
	if (s == -1){
		fprintf(stdout , " serveur: Erreur création de socket\n");
		fflush(stdout);
		error("socket");close(s); exit(EXIT_FAILURE);
	}
	else{
		fprintf(stdout , " serveur: Socket créée %d\n", s);
		fflush(stdout);
	}
	return s;
}



struct sockaddr_in init_serv_addr(int port){
	struct sockaddr_in sin;
	memset(&sin,0,sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	return sin;
}

int do_bind(int server_sock, struct sockaddr_in sin){
	int b = bind(server_sock,(struct sockaddr*)&sin, sizeof(sin));
	return b;
}


int do_accept(int server_sock,struct sockaddr_in* c_sin){
	//	socklen_t* c_sin_size =(socklen_t*) sizeof(&c_sin);
	//	printf("%d",c_sin_size);
	//	fflush(stdout);
	int c_sin_size = sizeof(c_sin);
	int client_sock = accept(server_sock,(struct sockaddr*)&c_sin,&c_sin_size);
	return client_sock;
}

void do_read(int client_sock,int server_sock){
	int txt_size;
	int size_buff = 512;
	txt_size = recv(client_sock,buffer,size_buff,0);
	if(txt_size!=-1){
		printf(" server: MESSAGE FROM CLIENT: %s\n",buffer);
		fflush(stdout);
	}
	else{
		error("recv");close(server_sock); exit(EXIT_FAILURE);
	}

}

char* do_write(int client_sock,int server_sock){
	char* msg = buffer;
	int size_txt = strlen(msg);
	int s = send(client_sock,msg,size_txt,0);
	if(s!=-1){
//		printf(" serveur: taille du message: %d \n server: taille du message envoyé: %d\n",size_txt, s);
//		fflush(stdout);
	}
	else{
		error("send");close(server_sock);exit(EXIT_FAILURE);
	}
	return msg;
}

void clean_up_client_socket(int client_sock,int server_sock){
	int s = close(client_sock);
	if(s!=-1){
		printf(" server: END OF COMMUNICATION WITH CLIENT\n");
		fflush(stdout);
	}
	else{
		error("send");close(server_sock);exit(EXIT_FAILURE);
	}
}

void clean_up_server_socket(int server_sock){
	int s = close(server_sock);
	if(s!=-1){
		printf(" server: CLOSED\n");
		fflush(stdout);
	}
	else{
		error("send");exit(EXIT_FAILURE);
	}
}

int main(int argc, char** argv){
	int server_sock;
	int client_sock;
	struct sockaddr_in sin = init_serv_addr(atoi(argv[1])); //init the serv_add structure
	struct sockaddr_in c_sin;
	int err; // error
	int k = 0;
	char* msg;


	if (argc != 2)
	{
		fprintf(stderr, "usage: RE216_SERVER port\n");
		return 1;
	}


	server_sock = do_socket();
	//Initis the pollfd structure
	fds[0].fd = server_sock;//create the socket, check for validity!
	fds[0].events = POLLIN;

	//perform the binding
	//we bind on the tcp port specified
	err = do_bind(server_sock,sin);
	if (err == 0){
		printf(" serveur: BIND: OK\n");
		fflush(stdout);
	}
	else{
		printf(" serveur: BIND: ERROR\n");
		fflush(stdout);
		error("bind");close(server_sock); exit(EXIT_FAILURE);
	}

	//specify the socket to be a server socket and listen for at most 20 concurrent client
	err = listen(server_sock,20);
	if(err == 0){
		printf(" serveur: LISTEN: OK\n");
		fflush(stdout);
	}
	else{
		printf(" serveur: LISTEN: ERROR\n");
		fflush(stdout);
		error("listen");close(server_sock); exit(EXIT_FAILURE);
	}

	printf(" serveur: en attente de connections \n\n");
	fflush(stdout);

	//accept connection from client
	client_sock = do_accept(server_sock,&c_sin);
	printf(" serveur : client socket: %d\n",client_sock);
	fflush(stdout);
	if(client_sock == -1){
		error("accept");close(server_sock); exit(EXIT_FAILURE);
	}
	else{
		printf(" serveur: CONECTION : reçu\n");
		fflush(stdout);
	}

	for (;;)
	{

		//read what the client has to say
		do_read(client_sock,server_sock);

		//we write back to the client
		msg=do_write(client_sock,server_sock);

		if(strcmp(msg,"/quit\n")==0){

			//clean up client socket
			clean_up_client_socket(client_sock,server_sock);

			//clean up server socket
			clean_up_server_socket(server_sock);
			return 0;

		}

	}



	return 0;
}

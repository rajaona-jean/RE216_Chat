#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>
char buffer[512];


void error(const char *msg)
{
	perror(msg);
	exit(1);
}

/*void get_addr_info(struct addrinfo hints,struct addrinfo *infoptr){
	int a = getaddrinfo("127.0.0.1","80",&hints,&infoptr);
	if(a == 0){
		printf(" client: get addr info: OK\n");
		fflush(stdout);
	}
	else{
    gai_strerror(errno);
		printf(" client: get addr info: ERREUR\n");
		fflush(stdout);
	}
}*/


int do_socket(){
	int s = socket(AF_INET,SOCK_STREAM,0);
	if (s == -1){
		fprintf(stdout , " client: Erreur création de socket 2\n");
		fflush(stdout);
		error("socket");close(s);exit(EXIT_FAILURE);
	}
	else{
		fprintf(stdout , " client: Socket créée %d\n", s);
		fflush(stdout);
	}
	return s;
}

struct sockaddr_in init_server_addr(int port,struct sockaddr_in server_sock){
	memset(&server_sock,'\0',sizeof(server_sock));
	server_sock.sin_family = AF_INET;
	server_sock.sin_port = htons(port);
	inet_aton("127.0.0.1",&server_sock.sin_addr);
	return server_sock;
}

void do_connect(int client_socket,struct sockaddr_in server_sock){
	int err = connect(client_socket,(struct sockaddr *)&server_sock,sizeof(server_sock));
	if(err == -1){
		error("connect");close(client_socket);exit(EXIT_FAILURE);
	}
}

//char* readline(char** msg){
//
//}

void handle_client_message(int client_socket,char* msg){
	int size_txt = strlen(msg);
	int a = send(client_socket,msg,size_txt,0);
	if(a!=-1){
//		printf(" client: taille du message: %d \n client: taille du message envoyé: %d\n",size_txt, a);
//		fflush(stdout);
	}
	else{
		error("send");close(client_socket);exit(EXIT_FAILURE);
	}
}

void do_read(int client_sock){
	int txt_size;
	int size_buff = 512;
	txt_size = recv(client_sock,buffer,size_buff,0);
	if(txt_size!=-1){
		printf(" client: MESSAGE FROM SERVER: %s\n",buffer);
		fflush(stdout);
	}
	else{
		error("recv");close(client_sock); exit(EXIT_FAILURE);
	}

}

int main(int argc,char** argv){
	int error;
	int client_sock;
	char* msg;
	struct addrinfo hints;
	struct addrinfo* infoptr;
	struct sockaddr_in server_sock;


	if (argc != 2)
	{
		fprintf(stderr,"usage: RE216_CLIENT port message\n argc = %d\n",argc);
		return 1;
	}

	fprintf(stderr,"usage: RE216_CLIENT port message\n",argc);

	//get address info from the server, useless for jalon 1
	//get_addr_info(hints,infoptr);

	//get the socket
	client_sock = do_socket();

	//init
	server_sock = init_server_addr(atoi(argv[1]),server_sock);


	//connect to remote socket
	do_connect(client_sock,server_sock);


	//get user input
	//buffer = readline(argv[3]);

	while(1){
		printf("\nPlease enter your line:\n");
		fflush(stdout);
		fgets(msg,512,stdin);
		//send message to the server
		handle_client_message(client_sock,msg);
		//read what the server has to say
		do_read(client_sock);

		if (strcmp(msg, "/quit\n") == 0)
			break;

	}

	return 0;


}

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

const int L = 512;
char buffer[512];
char* pseudo ;

void sigintHandler(int sig_num)
{
	signal(SIGINT, sigintHandler);
	printf("\n Cannot be terminated using Ctrl+C , do you mean /quit? \n");
	fflush(stdout);
}


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
		fprintf(stdout , " [client]: Erreur création de socket 2\n");
		fflush(stdout);
		error("socket");close(s);exit(EXIT_FAILURE);
	}
	else{
		fprintf(stdout , " [client]: Socket créée %d\n", s);
		fflush(stdout);
	}
	return s;
}

struct sockaddr_in init_server_addr(char* ip_addr,int port,struct sockaddr_in server_sock){
	memset(&server_sock,'\0',sizeof(struct sockaddr_in));
	server_sock.sin_family = AF_INET;
	server_sock.sin_port = htons(port);
	inet_aton(ip_addr,&server_sock.sin_addr);
	return server_sock;
}

void do_connect(int client_socket,struct sockaddr_in server_sock){
	int err = connect(client_socket,(struct sockaddr *)&server_sock,sizeof(struct sockaddr_in));
	if(err == -1){
		error("connect");close(client_socket);exit(EXIT_FAILURE);
	}
	else{
		printf(" Connecting to server... done!\n");
		fflush(stdout);
	}
}


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

char* do_read(int client_sock){
	int txt_size;
	int size_buff = L;
	txt_size = recv(client_sock,buffer,size_buff,0);
	if(txt_size!=-1 && strcmp(buffer,"1")!=0){
		printf(" [MESSAGE FROM SERVER]: %s\n",buffer);
		fflush(stdout);
	}
	else if(txt_size!=-1 && strcmp(buffer,"1")==0){
		printf(" [MESSAGE FROM SERVER]: Welcome on the chat %s\n",pseudo);
		fflush(stdout);
	}
	else{
		error("recv");close(client_sock); exit(EXIT_FAILURE);
	}
	return buffer;
}

char* get_nick_client(char * msg){
	int size = strlen(msg);
	char* nick = malloc(30*sizeof(char));
	int i;
	int fin;
	for(i=6;i<size-1;i++){ //size-1 car \n
		nick[i-6]=msg[i];
		fin = i;
	}
	return nick;
}

int main(int argc,char** argv){
	int error;
	int client_sock;
	char* msg = malloc(L*sizeof(char));
	struct addrinfo hints;
	struct addrinfo* infoptr;
	struct sockaddr_in server_sock;
	int first = 0;
	int first_connection = 0;


	if (argc != 3)
	{
		fprintf(stderr,"usage: RE216_CLIENT ip port\n argc = %d\n",argc);
		return 1;
	}


	//get address info from the server, useless for jalon 1
	//get_addr_info(hints,infoptr);

	//get the socket
	client_sock = do_socket();

	//init
	server_sock = init_server_addr(argv[1],atoi(argv[2]),server_sock);


	//connect to remote socket
	do_connect(client_sock,server_sock);




	while(1){

		if (first==0){
			msg=do_read(client_sock);

			if (strcmp(msg, "Server cannot accept incoming connections anymore. Try again later.") == 0)
				break;

			first=1;
		}
		memset (buffer, '\0', L);
		signal(SIGINT, sigintHandler);
		while(1){
			if(first_connection==0){
				printf("\nPlease enter your pseudo:\n");
				fflush(stdout);
				//get user input
				fgets(msg,L,stdin);
				pseudo = get_nick_client(msg);
				if (strcmp(msg, "/quit\n") == 0){
					strcpy(buffer, msg);
					handle_client_message(client_sock,msg);
					return 0;
				}

				//send message to the server
				handle_client_message(client_sock,msg);
				memset (buffer, '\0', L);
				//read what the server has to say
				do_read(client_sock);

				first_connection = atoi(buffer);
			}
			else if (first_connection==1){
				printf("\nPlease enter your line:\n");
				fflush(stdout);
				fgets(msg,L,stdin);

				if (strcmp(msg, "/quit\n") == 0){
					strcpy(buffer, msg);
					handle_client_message(client_sock,msg);
					return 0;
				}

				//send message to the server
				handle_client_message(client_sock,msg);
				memset (buffer, '\0', L);
				//read what the server has to say
				do_read(client_sock);

			}



		}

		return 0;

	}
}

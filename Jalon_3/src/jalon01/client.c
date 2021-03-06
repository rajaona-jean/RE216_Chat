#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <poll.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define MAX_USER 4

const int L = 512;


char* buffer;
char* pseudo;
int M= 2; //on compte le serveur


void sigintHandler(int sig_num)
{
	signal(SIGINT, sigintHandler);
	printf("\n Cannot be terminated using Ctrl+C , do you mean /quit? \n");
	fflush(stdout);
}

/*struct msgrecu{
//char *msg;
int sock ;
//int lenght ;
};


struct msgrecu *new_message(int sock)
{
struct msgrecu *new= malloc(sizeof(new));
new->sock = sock;
//new->msg = malloc(sizeof(char)*25);
//new->lenght  = 255;
return new;
}

//fonction pour le thread
void* recevoir(struct msgrecu *msge){
int a ;
for(;;)
{
a = recv(msge->sock,buffer,L,0);
if( a > 0){
printf(" [MESSAGE FROM SERVER]: %s\n",buffer);
fflush(stdout);
memset(buffer,0,L);
}
}
}*/

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
	server_sock.sin_addr.s_addr = inet_addr(ip_addr);
	//inet_aton(ip_addr,&server_sock.sin_addr);
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
	int txt_size ;
	int size_buff = L;
	txt_size = recv(client_sock,buffer,size_buff,0);
	if((txt_size!=-1) && (strcmp(buffer,"1")!=0) ){
		printf(" [MESSAGE FROM SERVER]: %s\n",buffer);
		fflush(stdout);
	}
	else if((txt_size!=-1) && (strcmp(buffer,"1")==0)){
		printf(" [MESSAGE FROM SERVER]: Welcome on the chat %s\n",pseudo);
		fflush(stdout);
	}
	else {
		/*printf(" [MESSAGE FROM SERVER]: %s\n",buffer);
		fflush(stdout);
		memset(buffer, 0, strlen(buffer));*/
		error("recv");close(client_sock); exit(EXIT_FAILURE);
	}

	return buffer;
}

char* do_read_who(int client_sock, int q){
	int txt_size;
	int size_buff = L;
	txt_size = recv(client_sock,buffer,size_buff,0);
	char * msg = malloc(L*sizeof(char));
	strcpy(msg,buffer);
	if(txt_size!=-1 && q==1 ){
		if(msg[strlen(msg)-1] == '1')
			msg[strlen(msg)-1] = '\0';
		printf(" [MESSAGE FROM SERVER]: %s",msg);
		fflush(stdout);
	}
	else if(txt_size!=-1 && msg[strlen(buffer)-1] == '1'){
		msg[strlen(msg)-1] = '\0';
		printf(" %s",msg);
		fflush(stdout);
	}
	else if(txt_size!=-1 && q==0){
		printf(" %s",msg);
		fflush(stdout);
	}

	else{
		error("recv");close(client_sock); exit(EXIT_FAILURE);
	}
	return msg;
}

//get the nickname  from the  message
char* get_nick_client(char * msg, short q){
	strcat(msg,"\0");
	int size = strlen(msg);
	char* nick = malloc((size-1)*sizeof(char));

	int i;
	for(i=0; i<size-1; i++)
		nick[i]='\0';

	if( q == 1 || q==8){//nick pseudo ou join
		for(i=6;i<size-1;i++){ //size-1 car \n
			nick[i-6]=msg[i];
		}
	}
	if( q == 4){//whois pseudo
		for(i=7;i<size-1;i++){ //size-1 car \n
			nick[i-7]=msg[i];
		}
	}
	if( q == 5 || q==7){//msgall msg ou create
		for(i=8;i<size-1;i++){ //size-1 car \n
			nick[i-8]=msg[i];
		}
	}
	return nick;
}


//check if the message starts with "/" and return the command
short if_slash_client(char* msg1){
	int size = strlen(msg1);
	char* msg = malloc(512*sizeof(char));
	strcpy(msg,msg1);
	char* cmd = malloc(7*sizeof(char));
	int i=0;
	int ok=0;
	for(i=0;i<7;i++){ //size-1 car \n
		cmd[i]='\0';
	}
	i=0;
	while(ok==0){
		if(msg[i]==' ' || msg[i]=='\n'){
			msg[i]='\0';
			ok = 1;
		}
		i++;
	}

	if(msg[0]=='/'){

		i=1;
		ok=0;
		while(ok==0){
			if(msg[i]==' ' || msg[i]=='\n'|| msg[i]=='\0'){
				cmd[i-1]='\0';
				ok = 1;
			}
			else{
				cmd[i-1]=msg[i];
			}
			i++;
		}

		if(strcmp(cmd,"nick")==0){
			free(msg);
			free(cmd);
			return 1;
		}
		if(strcmp(cmd,"who")==0){
			free(msg);
			free(cmd);
			return 2;
		}
		if(strcmp(cmd,"quit")==0){
			free(msg);
			free(cmd);
			return 3;
		}
		if(strcmp(cmd,"whois")==0){
			free(msg);
			free(cmd);
			return 4;
		}
		if(strcmp(cmd,"msgall")==0){
			free(msg);
			free(cmd);
			return 5;
		}
		if(strcmp(cmd,"create")==0){
			free(msg);
			free(cmd);
			return 7;
		}
		if(strcmp(cmd,"join")==0){
			free(msg);
			free(cmd);
			return 8;
		}
	}

	return 0;
}

///////////////////////////////////MAIN////////////////////////////////////////

int main(int argc,char** argv){
	int error;
	int client_sock;
	int first = 0;
	int first_connection = 0;
	int i;
	int* sl_check= malloc(sizeof(int));
	*sl_check = 0;

	short ok_from_serv=0;
	short stop =0;

	char* msg = malloc(L*sizeof(char));
	char* canal_name = malloc(50*sizeof(char));

	struct addrinfo hints;
	struct addrinfo* infoptr;
	struct sockaddr_in server_sock;


	buffer = malloc(L*sizeof(char));

	struct pollfd fdc[M];


	if (argc != 3)
	{
		fprintf(stderr,"usage: RE216_CLIENT ip port\n argc = %d\n",argc);
		return 1;
	}

	//get the socket
	client_sock = do_socket();
	//init
	server_sock = init_server_addr(argv[1],atoi(argv[2]),server_sock);
	//connect to remote socket
	do_connect(client_sock,server_sock);


	//pthread_t t;

	//Initis the pollfd structure

	fdc[0].fd = client_sock;
	fdc[0].events = POLLIN;
	fdc[1].fd = STDIN_FILENO;
	fdc[1].events = POLLIN;






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
			*sl_check=0;
			if(first_connection==0){
				printf("\nPlease enter your pseudo:\n> ");
				fflush(stdout);
				//get user input
				fgets(msg,L,stdin);

				*sl_check = if_slash_client(buffer);
				if(*sl_check!=0){

					if(*sl_check == 1){//nick
						pseudo = get_nick_client(msg,*sl_check);
						handle_client_message(client_sock,msg);
						memset (buffer, '\0', L);
						//read what the server has to say
						do_read(client_sock);
					}

					else if(*sl_check==3){ //quit
						strcpy(buffer, msg);
						handle_client_message(client_sock,msg);
						return 0;
					}

					else{
						memset (buffer, '\0', L);
						sprintf(buffer," Veuillez vous connecter pour effectuer cette action");
						handle_client_message(client_sock,msg);
						memset (buffer, '\0', L);
						//read what the server has to say
						do_read(client_sock);
					}


				}// Fin slash
				else{
					//send message to the server
					handle_client_message(client_sock,msg);
					memset (buffer, '\0', L);
					//read what the server has to say
					do_read(client_sock);
				}
				first_connection = atoi(buffer);
			}
			else if (first_connection==1){


				while(1){
					memset (buffer, '\0', L);
					printf("\nPlease enter your line:\n");
					printf("%s> ",pseudo);
					fflush(stdout);
					int p = poll(fdc,M,-1);
					memset (buffer, '\0', L);
					if(fdc[1].revents == POLLIN){

						fgets(msg,L,stdin);
						*sl_check = if_slash_client(buffer);
						if(*sl_check!=0){

							if(*sl_check == 2){//Who
								strcpy(buffer, msg);
								strcat(buffer,"\0");
								handle_client_message(client_sock,msg);
								memset (buffer, '\0', L);
								do_read_who(client_sock,1);
								while (buffer[strlen(buffer)-1] != '1'){
									memset (buffer, '\0', L);
									do_read_who(client_sock,0);
								}
							}

							else if(*sl_check==3){ //quit
								strcpy(buffer, msg);
								handle_client_message(client_sock,msg);
								return 0;
							}

							else if(*sl_check==7 || *sl_check==8){ //create ou join
								strcpy(buffer, msg);
								canal_name=get_nick_client(msg,*sl_check);
								handle_client_message(client_sock,msg);
								do_read(client_sock);
								while(stop == 0){
									printf("\nPlease enter your line:\n");
									printf("%s[%s]> ",pseudo,canal_name);
									fflush(stdout);
									p = poll(fdc,M,-1);

									if(fdc[1].revents == POLLIN){
										fgets(msg,L,stdin);
										handle_client_message(client_sock,msg);
										memset (buffer, '\0', L);
										//read what the server has to say
										do_read(client_sock);
										*sl_check=if_slash_client(buffer);
										if(*sl_check==3){ //quit
											strcpy(buffer, msg);
											handle_client_message(client_sock,msg);
											return 0;
										}
									}
									else if(fdc[0].revents == POLLIN){
										memset (buffer, '\0', L);
										do_read(client_sock);
									}
								}

							}
							else{
								//send message to the server
								handle_client_message(client_sock,msg);
								memset (buffer, '\0', L);
								//read what the server has to say
								do_read(client_sock);
							}

						}// Fin slash
						else if(*sl_check ==0){
							//send message to the server
							handle_client_message(client_sock,msg);
							memset (buffer, '\0', L);
							//read what the server has to say
							do_read(client_sock);

						}



					}//fin du else if first_conn ==1
					if(fdc[0].revents == POLLIN){
						memset (buffer, '\0', L);
						//read what the server has to say
						do_read(client_sock);


					}
				}//fin du while(1)

			}//fin else if connection
		}
	}





	return 0;

}

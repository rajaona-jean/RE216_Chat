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

#include "send.h"
#include "receive.h"


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
		error("recv");close(client_sock); exit(EXIT_FAILURE);
	}

	return buffer;
}

char* do_read_canal(int client_sock){
	int txt_size ;
	int size_buff = L;
	txt_size = recv(client_sock,buffer,size_buff,0);
	if(txt_size!=-1){
		printf(" %s\n",buffer);
		fflush(stdout);
	}
	else {

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

	if( q == 1 || q==8 || q==3 || q==9){//nick pseudo ou join ou quit
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

char* get_path(char * msg, int q){
	strcat(msg,"\0");
	int size = strlen(msg);
	char* nick = malloc(size*sizeof(char));
	short ok = 0;
	int c=0;
	int i;
	char* path = malloc(size*sizeof(char));
	memset(path,'\0',size);

	for(i=0; i<size; i++)
		nick[i]='\0';

	for(i=6;i<size;i++){ //size-1 car \n
		nick[i-6]=msg[i];
	}

	printf(" path, nick: %s\n",nick);
	fflush(stdout);

	for(i=0;i<strlen(nick);i++){
		if(ok == 0){
			if(nick[i] != ' '){
				c++;
			}

			else if(nick[i] == ' '){ //file's path
				ok =1;
			}
		}
		else if(ok ==1){
			path[i-c-1] = nick[i];
		}
	}

	strcat(nick,"\0");

	return path;
}

//check if the message starts with "/" and return the command
short if_slash_client(char* msg1){
	int size = strlen(msg1);
	char* msg = malloc(512*sizeof(char));
	memset(msg,'\0',512);
	strcpy(msg,msg1);
	char* cmd = malloc(7*sizeof(char));
	int i=0;
	int ok=0;
	for(i=0;i<7;i++){ //size-1 car \n
		cmd[i]='\0';
	}
	i=0;
	while(ok==0 && i<512){
		if(msg[i]==' ' || msg[i]=='\n'){
			msg[i]='\0';
			ok = 1;
		}
		i++;
	}

	if(msg[0]=='/'){

		i=1;
		ok=0;
		while(ok==0 && i<512){
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
		if(strcmp(cmd,"send")==0){
			free(msg);
			free(cmd);
			return 9;
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
	int k;
	int c;
	int* sl_check= malloc(sizeof(int));
	*sl_check = 0;

	short ok_from_serv=0;
	short stop =0;

	char* msg = malloc(L*sizeof(char));
	char* canal_name = malloc(50*sizeof(char));
	char* tmp = malloc(L*sizeof(char));
	char* path = malloc(512*sizeof(char));
	char* oth_pseudo= malloc(512*sizeof(char));
	char* message_send =malloc(512*sizeof(char));
	char* info = malloc(512*sizeof(char));
	char* ip_addr =  malloc(512*sizeof(char));
	char* port =  malloc(3*sizeof(char));

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

	//Initis the pollfd structure: manage several client

	fdc[0].fd = client_sock;
	fdc[0].events = POLLIN;
	fdc[1].fd = STDIN_FILENO;
	fdc[1].events = POLLIN;






	while(1){


		if (first==0){
			strcpy(msg,do_read(client_sock));

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
				*sl_check = if_slash_client(msg);
				if(*sl_check!=0){

					if(*sl_check == 1){//nick
						pseudo = get_nick_client(msg,*sl_check);
						handle_client_message(client_sock,msg);
						memset (buffer, '\0', L);
						//read what the server has to say
						do_read(client_sock);
					}

					else if(*sl_check==3){ //quit
						handle_client_message(client_sock,msg);
						return 0;
					}

					else{
						sprintf(msg,"Veuillez vous connecter pour effectuer cette action");
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
						*sl_check = if_slash_client(msg);
						if(*sl_check!=0){

							if(*sl_check == 2){//Who
								strcat(msg,"\0");
								handle_client_message(client_sock,msg);
								memset (buffer, '\0', L);
								do_read_who(client_sock,1);
								while (buffer[strlen(buffer)-1] != '1'){
									memset (buffer, '\0', L);
									do_read_who(client_sock,0);
								}
							}

							else if(*sl_check==3){ //quit

								handle_client_message(client_sock,msg);
								return 0;
							}

							else if(*sl_check==7 || *sl_check==8){ //create ou join
								strcat(msg,"\0");
								stop = 0;
								canal_name=get_nick_client(msg,*sl_check);
								handle_client_message(client_sock,msg);
								do_read(client_sock);

								if(msg[strlen(msg)-1]=='.'){
									stop = 1;
								}

								while(stop == 0){
									printf("\nPlease enter your line:\n");
									printf("%s[%s]> ",pseudo,canal_name);
									fflush(stdout);
									p = poll(fdc,M,-1);

									if(fdc[1].revents == POLLIN){//client is writting
										fgets(msg,L,stdin);
										handle_client_message(client_sock,msg);
										*sl_check=if_slash_client(msg);

										if(*sl_check==3){ //quit
											tmp = get_nick_client(msg,*sl_check);
											if(strcmp(tmp,canal_name)==0){
												stop = 1;
											}
										}
									}
									else if(fdc[0].revents == POLLIN){
										memset (buffer, '\0', L);
										strcpy(msg,do_read_canal(client_sock));
										if(strcmp(msg,tmp)==0){
											stop = 1;
										}
									}
								}

							}//fin create ou join

							else if (*sl_check == 9){ //send
								c=0;
								stop=0;
								strcat(msg,"\0");
								memset(message_send,'\0',512);
								memset(path,'\0',512);

								message_send = get_nick_client(msg,*sl_check);
								int length = strlen(message_send);

								for(k=0;k<length;k++){
									if(stop == 0){
										if(message_send[k] != ' '){
											oth_pseudo[k] = message_send[k]; //pseudo du desinataire
											c++;
										}
										else if(message_send[k] == ' '){ //fin pseudo
											stop =1;
										}
									}

									else if(stop ==1){
										path[k-c-1] = message_send[k]; //chemin du fichier
									}

								}
								printf(" Path: %s\n",path);
								fflush(stdout);
								handle_client_message(client_sock,msg);
								memset (buffer, '\0', L);
								do_read(client_sock);

								memset(msg,'\0',512);
								for(i=0;i<4;i++){
									msg[i]=buffer[i];
								}
								if (strcmp(msg,"Yes\n") == 0){
									printf(" File is sending..... \n");
									fflush(stdout);
									memset(info,'\0',512);
									memset(port,'\0',3);
									memset(ip_addr,'\0',512);
									strcpy(info,buffer);
									i = 0;
									c = 0;
									int m = 17;
									while(i<strlen(info)){
										if(info[i]==' '){
											c++;
										}
										i++;
										if(c==3){
											if(info[i]==' '){
												ip_addr[i-18]='\0';
											}
											else{
												ip_addr[i-18]=info[i];
											}
											m++;
										}
										if(c==4){
											if(info[i]=='\n'){
												port[i-m-1]='\0';
											}
											else{
												port[i-m-1]=info[i];
											}
										}

									}
									send_file(ip_addr,atoi(port),path);

								}
								else{
									printf("Sorry , %s refused the file \n",oth_pseudo);
									fflush(stdout);
								}

							}//fin send


							else{
								//send message to the server
								handle_client_message(client_sock,msg);
								memset (buffer, '\0', L);
								//read what the server has to say
								do_read(client_sock);

								//								if (strcmp(buffer,"Yes") == 0){
								//									printf("CLIENT RECEIVED SEND_FILE !!!!\n");
								//									fflush(stdout);
								//									int sock_client_2 ;
								//									memset (buffer, '\0', L);
								//									do_read(client_sock);
								//									printf("Buffer apres do_read client 222[%s]\n",buffer);
								//									fflush(stdout);
								//
								//
								//									//sprintf(buffer," %s ",path);
								//									send_file(/*path*/);
								//
								//								}
								//
								//								if (strcmp(msg, "no") == 0){
								//
								//									//appel de focntion receive_file
								//
								//								}
							}

						}// Fin slash
						else if(*sl_check ==0){
							//send message to the server
							handle_client_message(client_sock,msg);
							memset (buffer, '\0', L);
							//read what the server has to say
							do_read(client_sock);
							printf("msg [%s]\n",msg);
							fflush(stdout);


							if (strcmp(msg,"Yes\n") == 0){
								printf("msg apres yes [%s]\n",buffer);
								fflush(stdout);

								memset(info,'\0',512);
								strcpy(info,buffer);
								i = 0;
								c = 0;
								int m = 13;
								while(i<strlen(info)){
									if(info[i]==' '){
										c++;
									}
									i++;
									if(c==3){
										if(info[i]==' '){
											ip_addr[i-14]='\0';
										}
										else{
											ip_addr[i-14]=info[i];
										}
										m++;
									}
									if(c==4){
										port[i-m-1]=info[i];
									}

								}
								printf("ip_addr [%s], port [%d]\n",ip_addr,atoi(port));
								fflush(stdout);
								//path  port et adresse ip
								printf(" Where do you want to save the file ?\n  >");
								fflush(stdout);
								fgets(path,L,stdin);

								receive_file(ip_addr,atoi(port),path);
							}

						}



					}//fin du else if first_conn ==1
					if(fdc[0].revents == POLLIN){//others clients are speaking
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

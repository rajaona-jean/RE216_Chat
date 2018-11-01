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

#include "user.h"
#include "server.h"
#define MAX_USER 2

char buffer[512];
int N = MAX_USER+1; //on compte le serveur

struct sockaddr_in init_serv_addr(char* ip_addr,int port){
	struct sockaddr_in sin;
	memset(&sin,0,sizeof(struct sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	//sin.sin_addr.s_addr = htonl(INADDR_ANY);
	//inet_aton(ip_addr,&sin.sin_addr);
	sin.sin_addr.s_addr = inet_addr(ip_addr);
	return sin;
}

void setsock(int socket_fd){
	int option = 1;
	int error = setsockopt(socket_fd,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option));
	if(error == -1){
		perror("setsockopt");
	}
}

void sigintHandler(int sig_num)
{
	//signal(SIGINT, sigintHandler);
	printf("\n Cannot be terminated using Ctrl+C \n");
	fflush(stdout);
}


void error(const char *msg)
{
	perror(msg);
	exit(1);

}

int do_socket(){
	int s = socket(AF_INET,SOCK_STREAM,0);
	setsock(s);
	if (s == -1){
		fprintf(stdout , " [serveur]: Erreur création de socket\n");
		fflush(stdout);
		close(s);error("socket");
	}
	else{
		fprintf(stdout , " [serveur]: Socket créée %d\n", s);
		fflush(stdout);
	}
	return s;
}


int do_bind(int server_sock, struct sockaddr_in sin){
	int b = bind(server_sock,(struct sockaddr*)&sin, sizeof(struct sockaddr_in));
	if (b == 0){
		printf(" [serveur]: BIND: OK\n");
		fflush(stdout);
	}
	else{
		printf(" [serveur]: BIND: ERROR\n");
		fflush(stdout);
		close(server_sock); error("bind");
	}
	return b;
}

int do_listen(int server_sock,int connect_nb){
	int err;
	err = listen(server_sock,20);
	if(err == 0){
		printf(" [serveur]: LISTEN: OK\n");
		fflush(stdout);

	}
	else{
		printf(" [serveur]: LISTEN: ERROR\n");
		fflush(stdout);
		close(server_sock); error("listen");
	}
	return 0;
}

int do_accept(int server_sock,struct sockaddr_in* c_sin,int q){
	//	socklen_t* c_sin_size =(socklen_t*) sizeof(&c_sin);
	//	printf("%d",c_sin_size);
	//	fflush(stdout);
	int c_sin_size = sizeof(struct sockaddr_in);
	printf("do_accept: %d ", c_sin_size);
	fflush(stdout);
	int client_sock = accept(server_sock,(struct sockaddr*)&c_sin,&c_sin_size);
	if(client_sock == -1){
		close(server_sock); error("accept");
	}
	else if(q==0){
		printf(" [serveur] : client socket: %d\n",client_sock);
		fflush(stdout);
		printf(" [serveur]: CONECTION : reçu\n");
		fflush(stdout);
	}
	return client_sock;
}

char* do_read(int client_sock,int server_sock,struct sockaddr_in* sin, int q){
	int txt_size;
	int size_buff = 512;
	char* msg = buffer;
	txt_size = recv(client_sock,buffer,size_buff,0);
	if(txt_size!=-1 && q == 1){
		printf(" [MESSAGE FROM CLIENT %d]: %s\n",client_sock,buffer);
		fflush(stdout);
	}
	else if (txt_size!=-1){
		close(server_sock);error("recv");
	}
	return msg;
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
		close(server_sock);error("send");
	}
	return msg;
}

void clean_up_client_socket(int client_sock,int server_sock, int q){
	int s = close(client_sock);
	if((s!=-1) && (q==0)){
		printf(" [server]: END OF COMMUNICATION WITH CLIENT %d\n",client_sock);
		fflush(stdout);
	}
	else if (s == -1){
		close(server_sock);error("send");
	}
}
void clean_up_server_socket(int server_sock){
	int s = close(server_sock);
	if(s!=-1){
		printf(" [server]: CLOSED\n");
		fflush(stdout);
	}
	else{
		error("send");
	}
}

//get the nickname  from the  message
char* get_nick(char * msg, short q){
	int size = strlen(msg);
	char* nick = malloc((size-1)*sizeof(char));

	int i;
	for(i=0; i<size-1; i++)
		nick[i]='\0';

	if( q == 1){//nick pseudo
		for(i=6;i<size-1;i++){ //size-1 car \n
			nick[i-6]=msg[i];
		}
	}
	if( q == 4){//whois pseudo
		for(i=7;i<size-1;i++){ //size-1 car \n
			nick[i-7]=msg[i];
		}
	}
	if( q == 5){//msgall msg
		for(i=8;i<size-1;i++){ //size-1 car \n
			nick[i-8]=msg[i];
		}
	}
	return nick;
}

//check if the message starts with "/nick" to edit the nickname
short if_slash(char* msg1){
	int size = strlen(msg1);
	char* msg = malloc(512*sizeof(char));
	strcpy(msg,msg1);
	char* cmd = malloc(7*sizeof(char));
	int i=0;
	int ok=0;
	for(i=0;i<6;i++){ //size-1 car \n
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



	printf(" if_slash : %s\n",msg);
	fflush(stdout);
	if(msg[0]=='/'){
		printf(" SLASH !!!!!\n");
		fflush(stdout);

		i=1;
		ok=0;
		while(ok==0){
			if(msg[i]==' ' || msg[i]=='\n'|| msg[i]=='\0'){
				cmd[i]='\0';
				ok = 1;
			}
			else{
				cmd[i-1]=msg[i];
			}
			i++;
		}

		if(strcmp(cmd,"nick")==0){
			printf(" NICK !!!!!\n");
			fflush(stdout);
			return 1;
		}
		if(strcmp(cmd,"who")==0){
			printf(" WHO !!!!!\n");
			fflush(stdout);
			return 2;
		}
		printf(" strcmp: %d\n",strcmp(cmd,"quit\n"));
		fflush(stdout);
		if(strcmp(cmd,"quit")==0){
			printf(" QUIT !!!!!\n");
			fflush(stdout);
			return 3;
		}
		if(strcmp(cmd,"whois")==0){
			printf(" WHOIS !!!!!\n");
			fflush(stdout);
			return 4;
		}
		if(strcmp(cmd,"msgall")==0){
			printf(" MSGALL !!!!!\n");
			fflush(stdout);
			return 5;
		}

	}

	return 0;
}




/////////////////////////MAIN//////////////////////////////////////////////////

int main(int argc, char** argv){
	int server_sock;
	int client_sock;
	struct pollfd fds[N];
	struct Liste* liste = malloc(sizeof(struct Liste*));

	struct sockaddr_in sin = init_serv_addr(argv[1],atoi(argv[2])); //init the serv_add structure
	struct sockaddr_in c_sin;
	memset(&c_sin, 0, sizeof(c_sin));
	int k = 0;

	int i;
	int j=0;
	int ok = 0;
	int p;
	int connect = 0;
	int user_nb = nb_of_user();
	short verify_nick=0;
	short sl_check=0;
	short pseudo_of_sock = 0;


	char* pseudo;
	char* oth_user;
	char* msg_all;
	char* msg = malloc(512*sizeof(char));
	char** user_liste = malloc(user_nb*sizeof(char*));
	for(i=0; i<user_nb;i++){
		user_liste[i] = malloc(28*sizeof(char));
	}



	liste=init(3);
	init_users(liste,user_nb,user_liste);
	fill_users(liste,user_nb,user_liste);

	see_user(liste);
	if (argc != 3)
	{
		fprintf(stderr, "usage: RE216_SERVER ip port\n");
		return 1;
	}

	//create the socket, check for validity!
	server_sock = do_socket();

	//perform the binding
	//we bind on the tcp port specified
	do_bind(server_sock,sin);

	//specify the socket to be a server socket and listen for at most 20 concurrent client
	do_listen(server_sock,N+1);

	printf(" [serveur]: en attente de connections \n\n");
	fflush(stdout);

	//Initis the pollfd structure
	for(i=0;i<N;i++){
		fds[i].fd = 0;
		fds[i].revents = 0;
		fds[i].events = POLLIN;
	}

	fds[0].fd = server_sock;
	fds[0].events = POLLIN;

	signal(SIGINT, sigintHandler);

	for (;;)
	{

		while(1){

			memset(buffer, '\0', 512);
			p = poll(fds,N,-1);

			for(i=0;i<N;i++){ // Gestion event (qui crée l'event ?)
				if(fds[i].revents == POLLIN){// On voit ce que veut le client

					if(i==0){// On accepte la connexion
						//et on cherche le bon emplacement pour ajouter le client dans le fds

						client_sock = do_accept(server_sock,&c_sin,0);
						ok = 0;
						j = 0;
						while(ok==0 && j<N){// On regarde si il y a de la place pour un nouveau client
							j++;
							if (fds[j].fd == 0){
								fds[j].fd = client_sock ;
								ok = 1;
							}
						}

						if(ok == 1){// tout est ok on demande le pseudo
							memset(buffer,'\0',512);
							char* accept="Please logon with /nick <your pseudo>";
							strcpy(buffer,accept);
							do_write(client_sock,server_sock);
							memset(buffer,'\0',512);
						}


					}

					if((i==0) && ok==0){ // Trop de connexions
						memset(buffer,'\0',512);
						char* no_accept="Server cannot accept incoming connections anymore. Try again later.";
						strcpy(buffer,no_accept);
						do_write(client_sock,server_sock);
						memset(buffer,'\0',512);
						clean_up_client_socket(client_sock,server_sock,1);
					}



					if(i!=0){ // On parle avec le client
						memset(buffer,'\0',512);
						client_sock = fds[i].fd ;
						pseudo_of_sock = pseudo_from_sock(liste,client_sock);
						do_read(client_sock,server_sock,&sin,1);
						sl_check = if_slash(buffer);
						printf(" buffer : %s\n",buffer);
						fflush(stdout);
						if(sl_check!=0){
							if(sl_check == 1){// On verifie /nick

								if(see_connected_user(liste,client_sock,server_sock,0) == 0){//Le client n'est pas connecté

									printf(" buffer 2: %s\n",buffer);
									fflush(stdout);
									pseudo = get_nick(buffer,sl_check);
									printf(" Pseudo : %s\n",pseudo);
									fflush(stdout);

									memset(buffer, '\0', 512);
									verify_nick = verify_pseudo(liste,pseudo);
									printf(" verify_nick : %d\n",verify_nick);
									fflush(stdout);
									if(verify_nick == 1){ // On reconnait le client // Bon pseudo

										set_info(liste,pseudo,client_sock,c_sin);
										memset(buffer, '\0', 512);
										strcpy(buffer,"1");
										printf(" envoie client : %s\n",buffer);
										fflush(stdout);
										do_write(client_sock,server_sock);

										memset(buffer, '\0', 512);

									}
									else{ // On connait pas le client // Mauvais pseudo
										strcpy(buffer,"Try to reconnect");
										msg=do_write(client_sock,server_sock);
										memset(buffer,'\0',512);
									}

								}
								else{ // Le client est deja connecté, on change le pseudo
									msg = get_nick(buffer,sl_check);
									edit_pseudo_from_sock(liste,client_sock,msg);
									memset(buffer,'\0',512);
									strcpy(buffer, "Nouveau pseudo: ");
									strcat(buffer,msg);
									msg=do_write(client_sock,server_sock);
								}//fin changement pseudo
							}// Fin /nick

							if(sl_check == 2){//Who
								memset(buffer,'\0',512);
								strcpy(buffer,"Connected users:\n");
								do_write(client_sock,server_sock);
								see_connected_user(liste,client_sock,server_sock,1);
								memset(buffer,'\0',512);
								strcpy(buffer,"1");

								while(buffer[strlen(buffer)-1] != '1'){;}
								//we write back to the client
								msg=do_write(client_sock,server_sock);
								printf("write\n");
								fflush(stdout);
							}

							if(sl_check==3){ //quit
								for(i=1;i<N;i++){
									if(fds[i].fd == client_sock)
										fds[i].fd = 0;
								}
								if(pseudo_from_sock(liste,client_sock)==1){
									down_connect(liste,client_sock);
									down_client_sock(liste,client_sock);
								}
								//memset (msg, 0, sizeof(msg));
								//clean up client socket
								clean_up_client_socket(client_sock,server_sock,0);


							}
							if(sl_check==4){//whois
								oth_user = get_nick(buffer,sl_check);
								get_info(liste, oth_user, client_sock, server_sock);
							}
							if(sl_check==5){//msgall
								ok=0;
								msg_all = get_nick(buffer,sl_check);
								memset(buffer,'\0',512);
								sprintf(buffer," [%s]: ", pseudo);
								strcat(buffer,msg_all);
								strcat(buffer,"\n");
								for(i=0; i<N;i++){//on envoie à tous le monde
									if(fds[i].fd != client_sock && fds[i].fd != 0){// on verifie qu'on envoie pas à l'expéditeur
										printf(" msgall: %d\n", fds[i].fd);
										fflush(stdout);
										do_write(fds[i].fd,server_sock);
										ok=1;
										memset(buffer,'\0',512);
									}
									if(ok==0){
										memset(buffer,'\0',512);
										strcpy(buffer, "[SERVER]: No one else is connected");
										do_write(client_sock,server_sock);
										memset(buffer,'\0',512);
									}
									if(ok==1){
										memset(buffer,'\0',512);
										strcpy(buffer, "[SERVER]: Message send");
										do_write(client_sock,server_sock);
										memset(buffer,'\0',512);
									}
								}
							}

						}// Fin slash
						else if(sl_check ==0){
							msg=do_write(client_sock,server_sock);
							memset(buffer,'\0',512);
						}

					} // Fin du dialogue
				}// Fin d

			} // end of if, gestion evenement


		} //end of for
	}//end of while



	return 0;
}


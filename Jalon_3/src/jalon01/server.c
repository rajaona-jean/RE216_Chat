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

#include "user.h"
#include "server.h"

char buffer[512];
int N = 3;

struct sockaddr_in init_serv_addr(int port){
	struct sockaddr_in sin;
	memset(&sin,0,sizeof(struct sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
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
	int b = bind(server_sock,(struct sockaddr*)&sin, sizeof(sin));
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
	int c_sin_size = sizeof(c_sin);
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
	else if ((s!=-1) && (q==1)){

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
char* get_nick(char * msg){
	int size = strlen(msg);
	char* nick = malloc(30*sizeof(char));
	int i;
	for(i=6;i<size-1;i++){ //size-1 car \n
		nick[i-6]=msg[i];
	}
	return nick;
}

//check if the message starts with "/nick" to edit the nickname
short if_slash(char* msg){
	int size = strlen(msg);
	char* cmd = malloc(6*sizeof(char));
	int i;
	printf(" if_slash : %s\n",msg);
	fflush(stdout);
	if(msg[0]=='/'){
		printf(" SLASH !!!!!\n");
		fflush(stdout);
		for(i=1;i<5;i++){ //size-1 car \n
			cmd[i-1]=msg[i];
		}
		if(strcmp(cmd,"nick")==0){
			printf(" NICK !!!!!\n");
			fflush(stdout);
			return 1;
		}
		if(strcmp(cmd,"who\n")==0){
			printf(" WHO !!!!!\n");
			fflush(stdout);
			return 2;
		}

	}

	printf(" RIEN !!!!!\n");
	fflush(stdout);
	return 0;
}


//short if_who(char* msg){
//	int size = strlen(msg);
//	char* _who = malloc(6*sizeof(char));
//	int i;
//	for(i=0;i<4;i++){ //size-1 car \n
//		_who[i]=msg[i];
//	}
//
//	if(strcmp(_who,"/who")==0){
//		return 1;
//	}
//
//	return 0;
//}



/////////////////////////MAIN//////////////////////////////////////////////////

int main(int argc, char** argv){
	int server_sock;
	int client_sock;
	struct pollfd fds[N];
	struct Liste* liste = malloc(sizeof(liste));

	struct sockaddr_in sin = init_serv_addr(atoi(argv[1])); //init the serv_add structure
	struct sockaddr_in c_sin;
	int k = 0;
	char* msg = malloc(512*sizeof(char));
	int i;
	int j=0;
	int ok = 0;
	int p;
	int connect = 0;
	short verify_nick=0;


	char* pseudo;
	int user_nb = nb_of_user();
	char user_liste[user_nb][28];
	short sl_check=0;
	short pseudo_of_sock = 0;

	liste=init(3);
	init_users(liste,user_nb,user_liste);
	fill_users(liste,user_nb,user_liste);

	see_user(liste);
	if (argc != 2)
	{
		fprintf(stderr, "usage: RE216_SERVER port\n");
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

					if((i==0) && (fds[N-1].fd==0)){// On accepte la connexion
						//et on cherche le bon emplacement pour ajouter le client dans le fds

						client_sock = do_accept(server_sock,&c_sin,0);

						memset(buffer,'\0',512);
						char* accept="Please enter your pseudo";
						strcpy(buffer,accept);
						do_write(client_sock,server_sock);
						memset(buffer,'\0',512);
						ok = 0;

						while(ok==0){
							j++;
							if (fds[j].fd == 0){
								fds[j].fd = client_sock ;
								ok = 1;
								j=0;
							}
						}

					}

					else if((i==0) && (fds[N-1].fd!=0)){ // Trop de connexions
						client_sock = do_accept(server_sock,&c_sin,1);
						memset(buffer,'\0',512);
						char* no_accept="Server cannot accept incoming connections anymore. Try again later.";
						strcpy(buffer,no_accept);
						do_write(client_sock,server_sock);
						memset(buffer,'\0',512);
						clean_up_client_socket(client_sock,server_sock,1);
					}



					if(i!=0){ // On parle avec le client

						client_sock = fds[i].fd ;
						pseudo_of_sock = pseudo_from_sock(liste,client_sock);
						do_read(client_sock,server_sock,&sin,1);
						sl_check = if_slash(buffer);
						printf(" buffer : %s\n",msg);
						fflush(stdout);
						if(sl_check!=0){
							if(sl_check == 1){// On verifie /nick

								if(see_connected_user(liste,client_sock,server_sock,0) == 0){//Le client n'est pas connecté

									printf(" buffer 2: %s\n",buffer);
									fflush(stdout);
									pseudo = get_nick(buffer);
									printf(" Pseudo : %s\n",pseudo);
									fflush(stdout);

									memset(buffer, '\0', 512);
									verify_nick = verify_pseudo(liste,pseudo);
									printf(" verify_nick : %d\n",verify_nick);
									fflush(stdout);
									if(verify_nick == 1){ // On reconnait le client // Bon pseudo
										set_connect(liste,pseudo); // On connecte le client

										memset(buffer, '\0', 512);
										strcpy(buffer,"1");
										printf(" envoie client : %s\n",buffer);
										fflush(stdout);
										do_write(client_sock,server_sock);

										memset(buffer, '\0', 512);
										set_client_sock(liste,pseudo,client_sock);
									}
									else{ // On connait pas le client // Mauvais pseudo
										strcpy(buffer,"Try to reconnect");
										msg=do_write(client_sock,server_sock);
										memset(buffer,'\0',512);
									}

								}
								else{ // Le client est deja connecté, on change le pseudo
									msg = get_nick(buffer);
									edit_pseudo_from_sock(liste,client_sock,msg);
									msg=do_write(client_sock,server_sock);
								}//fin changement pseudo
							}// Fin /nick
							if(sl_check == 2){
								see_connected_user(liste,client_sock,server_sock,1);
								//we write back to the client
								msg=do_write(client_sock,server_sock);
							}

						}// Fin slash
						else{
							msg=do_write(client_sock,server_sock);
							memset(buffer,'\0',512);
						}

						/*if(pseudo_of_sock == 1){ // Parle tranquillement si le client est connecté
							do_read(client_sock,server_sock,&sin,1);
							if(if_nick(buffer)==0){
								//we write back to the client
								msg=do_write(client_sock,server_sock);
								//memset(buffer,0,512);
							}
							else{
								msg = get_nick(buffer);
								edit_pseudo_from_sock(liste,client_sock,msg);
								msg=do_write(client_sock,server_sock);
							}


							if(if_who(buffer)==1){
								see_connected_user(liste,client_sock,server_sock);
								//we write back to the client
								msg=do_write(client_sock,server_sock);
							}


						}*/

						} // Fin du dialogue
					}// Fin d

				} // end of if, gestion evenement

				if(strcmp(msg,"/quit\n")==0){ // Si on lit /quit
					for(i=1;i<N;i++){
						if(fds[i].fd == client_sock)
							fds[i].fd = 0;
					}
					down_connect(liste,client_sock);
					down_client_sock(liste,client_sock);
					//memset (msg, 0, sizeof(msg));
					//clean up client socket
					clean_up_client_socket(client_sock,server_sock,0);


				}


			} //end of for
		}//end of while



		return 0;
}


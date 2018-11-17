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


#include "canal.h"
#include "user.h"
#include "server.h"
#define MAX_USER 4

char buffer[512];
int N = MAX_USER+1; //on compte le serveur

struct sockaddr_in init_serv_addr(char* ip_addr,int port){
	struct sockaddr_in sin;
	//memset(&sin,0,sizeof(struct sockaddr_in));
	memset(&sin,0,sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	//inet_aton(ip_addr,&sin.sin_addr);
	//sin.sin_addr.s_addr = inet_addr(ip_addr);
	return sin;
}

void setsock(int socket_fd){
	int option = 1;
	int error = setsockopt(socket_fd,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option));
	if(error == -1){
		perror("setsockopt");
	}
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
	int client_sock = accept(server_sock,(struct sockaddr*)c_sin,&c_sin_size);
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
	strcat(msg,"\0");
	int size = strlen(msg);
	char* nick = malloc(size*sizeof(char));

	int i;
	for(i=0; i<size; i++)
		nick[i]='\0';

	if( q == 1 || q == 8 || q==3 || q == 9){//nick pseudo ou join canal_name ou quit canal_name ou send
		for(i=6;i<size-1;i++){ //size-1 car \n
			nick[i-6]=msg[i];
		}
	}
	if( q == 4){//whois pseudo
		for(i=7;i<size-1;i++){ //size-1 car \n
			nick[i-7]=msg[i];
		}
	}
	if( q == 5 || q==7){//msgall msg
		for(i=8;i<size-1;i++){ //size-1 car \n
			nick[i-8]=msg[i];
		}
	}

	if( q == 6){//msgall msg
		for(i=5;i<size-1;i++){ //size-1 car \n
			nick[i-5]=msg[i];
		}
	}

	return nick;
}
char* get_path(char * msg, int q){
	strcat(msg,"\0");
	int size = strlen(msg);
	char* nick = malloc(size*sizeof(char));

	int i;
	for(i=0; i<size; i++)
		nick[i]='\0';

	for(i=6;i<size;i++){ //size-1 car \n
		nick[i-6]=msg[i];
	}

	nick = get_nick(nick,q);
	strcat(nick,"\0");

	return nick;
}



//check if the message starts with "/nick" or "/who" or "/whois" or "/msgall" or "/quit"
short if_slash(char* msg1){
	int size = strlen(msg1);
	char* msg = malloc(512*sizeof(char));
	strcpy(msg,msg1);
	char* cmd = malloc(7*sizeof(char));
	int i=0;
	short ok=0;
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
	//printf(" if_slash : %s\n",msg);
	//fflush(stdout);
	//check if the message starts with "/"
	if(msg[0]=='/'){
		//printf(" SLASH !!!!!\n");
		//fflush(stdout);

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
		//printf(" strcmp: %d\n",strcmp(cmd,"quit\n"));
		//fflush(stdout);
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
		if(strcmp(cmd,"msg")==0){
			return 6;
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


void speak_with_canal(struct Liste* liste,struct First_c* c_liste,char* pseudo, char* canal_name, int server_sock){
	struct Canals* previous;
	struct Canals* cur_canal;
	int find1 = 0 ;
	int stop = 0;
	int i=0;
	int client_sock=0;


	if (c_liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = first_canal(c_liste);
	cur_canal = next_canal(previous);
	if(cur_canal == NULL)
		stop=1;

	while(find1!=1 && stop!=1){
		if(!strcmp(name_canal(cur_canal),canal_name)){
			find1 = 1;
			i = 0;
			while(i<nb_canal_members(cur_canal)){
				if((strcmp(members_canal_i(cur_canal,i),"\0"))!=0 && (strcmp(members_canal_i(cur_canal,i),pseudo)!=0)){
					client_sock=sock_from_pseudo(liste,members_canal_i(cur_canal,i));
					do_write(client_sock,server_sock);

				}
				i++;
			}
		}
		previous = cur_canal;
		cur_canal = next_canal(cur_canal);

		if(cur_canal == NULL)
			stop=1;
	}
}

void deco_members_from_canal(struct Liste* liste,struct First_c* c_liste, char* canal_name){
	struct Canals* previous;
	struct Canals* cur_canal;
	int find1 = 0 ;
	int stop = 0;
	int i = 1; //not the creator of the canal


	if (c_liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = first_canal(c_liste);// c'est le canal global
	cur_canal = next_canal(previous);

	while(find1!=1 && stop!=1){
		if(!strcmp(name_canal(cur_canal),canal_name)){
			find1 = 1;
			while(i < nb_canal_members(cur_canal)){
				if(strcmp(members_canal_i(cur_canal,i),"\0")!=0)
					set_canal_name(liste,members_canal_i(cur_canal,i),"global");
				i++;
			}
		}

		cur_canal = next_canal(cur_canal);

		if(cur_canal == NULL)
			stop=1;
	}
}

void quit_canal(struct Liste* liste,struct First_c* c_liste,char* pseudo, char* canal_name, int server_sock){
	struct Canals* previous;
	struct Canals* cur_canal;
	int find1 = 0 ;
	int stop = 0;
	int i = 0;
	int nb_members_in_canal=0;


	if (c_liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = first_canal(c_liste);// c'est le canal global
	cur_canal = next_canal(previous);

	while(find1!=1 && (cur_canal!=NULL)){
		if(!strcmp(name_canal(cur_canal),canal_name)){
			find1 = 1;
			while(i<nb_canal_members(cur_canal)){
				if(strcmp(members_canal_i(cur_canal,i),"\0")!=0){
					nb_members_in_canal++;
				}
				i++;
			}
			i=0;
			while(stop==0 && i<nb_canal_members(cur_canal)){
				if(strcmp(members_canal_i(cur_canal,i),pseudo)==0){
					set_canal_members_name_i(cur_canal,"\0",i);
					set_canal_name(liste,pseudo,"global");
					speak_with_canal(liste,c_liste,pseudo,canal_name,server_sock);
					stop=1;
					if(nb_members_in_canal==1){
						del_canal(c_liste,canal_name);
					}
				}
				i++;
			}
		}
		cur_canal = next_canal(cur_canal);
	}
}

/////////////////////////MAIN//////////////////////////////////////////////////

int main(int argc, char** argv){
	int server_sock;
	int client_sock;
	struct pollfd fds[N];
	struct Liste* liste = malloc(sizeof(struct Liste*));
	struct First_c* c_liste = malloc(sizeof(struct First_c*));

	struct sockaddr_in sin = init_serv_addr(argv[1],atoi(argv[2])); //init the serv_add structure
	struct sockaddr_in c_sin;
	//memset(&c_sin, 0, sizeof(c_sin));

	int i;
	int j=0;
	int p;
	int connect = 0;
	int user_nb = nb_of_user();
	int k=0;
	int c=0;
	int stop=0;
	short verify_nick=0;
	short sl_check=0;
	short pseudo_of_sock = 0;
	short ok = 0;


	char* oth_pseudo =malloc(512*sizeof(char)); //pseudo de destination
	char* message = malloc(512*sizeof(char)) ; //message a envoyer
	char* tmp = malloc(512*sizeof(char)) ;
	char* canal_name = malloc(30*sizeof(char));

	char* pseudo = "";
	char* oth_user;
	char* path;
	char* msg_all;
	char* _accept;

	char* pseudo_destinateur = malloc(512*sizeof(char));
	char* msg = malloc(512*sizeof(char));
	char** user_liste = malloc(user_nb*sizeof(char*));
	for(i=0; i<user_nb;i++){
		user_liste[i] = malloc(28*sizeof(char));
	}



	liste=init(3);
	init_users(liste,user_nb,user_liste);
	fill_users(liste,user_nb,user_liste);
	c_liste=init_canal(user_liste,user_nb);

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


	for (;;)
	{

		while(1){

			memset(buffer, '\0', 512);
			memset(tmp, '\0',512);
			pseudo="";
			canal_name="";
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
							_accept="Please login with /nick <your pseudo>";
							strcpy(buffer,_accept);
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
						pseudo_of_sock = pseudo_known(liste,client_sock);
						do_read(client_sock,server_sock,&sin,1);
						sl_check = if_slash(buffer);
						printf(" buffer : %s\n",buffer);
						fflush(stdout);
						if(sl_check!=0){
							if(sl_check == 1){// On verifie /nick

								if(see_connected_user(liste,client_sock,server_sock,0) == 0){//Le client n'est pas connecté

									//printf(" buffer 2: %s\n",buffer);
									//fflush(stdout);
									pseudo = get_nick(buffer,sl_check);
									printf(" Pseudo : %s\n",pseudo);
									fflush(stdout);

									memset(buffer, '\0', 512);
									verify_nick = verify_pseudo(liste,pseudo);
									//printf(" verify_nick : %d\n",verify_nick);
									//fflush(stdout);
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
							}//fin who

							if(sl_check==3){ //quit

								if(pseudo_known(liste,client_sock)==0){// pas de pseudo relatif au num de socket
									do_write(client_sock,server_sock);
									for(i=1;i<N;i++){
										if(fds[i].fd == client_sock)
											fds[i].fd = 0;
									}

									clean_up_client_socket(client_sock,server_sock,0);
								}
								else{
									canal_name=get_canal_name_from_sock(liste,client_sock);
									if(strcmp(canal_name,"global")!=0){// quit canal
										msg = get_nick(buffer,sl_check);
										if(strcmp(msg,canal_name)==0){
											pseudo = get_pseudo_from_sock(liste,client_sock);
											memset(buffer,'\0',512);
											sprintf(buffer, " %s leaves the canal\n",pseudo);
											quit_canal(liste,c_liste,pseudo,canal_name,server_sock);
											memset(buffer,'\0',512);
										}
										else{
											memset(buffer,'\0',512);
											strcpy(buffer,"You can not quit with a wrong canal name");
											do_write(client_sock,server_sock);
											memset(buffer,'\0',512);

										}
									}
									else{ // quit

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
								}


							}//fin quit

							if(sl_check==4){//whois
								oth_user = get_nick(buffer,sl_check);
								get_info(liste, oth_user, client_sock, server_sock);
							}
							if(sl_check==5){//msgall

								int ok_msgall = 0;
								msg_all = get_nick(buffer,sl_check);
								char * sender = get_pseudo_from_sock(liste,client_sock);
								printf("qui envoie %s\n",sender);
								fflush(stdout);

								memset(buffer,'\0',512);
								sprintf(buffer," [%s]: ", sender);
								strcat(buffer,msg_all);
								strcat(buffer,"\n");


								for(i=0; i<N ;i++){//on envoie à tout le monde
									int s = fds[i].fd;
									if(s != 0){
										if((s != client_sock)  && (s > 4)){ // on verifie qu'on envoie pas à l'expéditeur
											printf(" msgall: %d\n", fds[i].fd);
											fflush(stdout);
											printf(" buffer apres msgall: %s\n", buffer);
											fflush(stdout);
											do_write(s,server_sock);
											ok_msgall=1;
											printf("ok_msgall is 1\n");
											fflush(stdout);
										}
									}
								}
								if(ok_msgall ==1){
									memset(buffer,'\0',512);
									strcpy(buffer, "[SERVER]: Message sent to all users");
									do_write(client_sock,server_sock);
									memset(buffer,'\0',512);
								}

								if(ok_msgall==0){
									printf("ok_msgall is 0\n");
									fflush(stdout);
									memset(buffer,'\0',512);
									strcpy(buffer, "[SERVER]: No one else is connected");
									do_write(client_sock,server_sock);
									memset(buffer,'\0',512);
								}
							}

							if(sl_check==6){ //msg

								char * message_msg= get_nick(buffer,sl_check); 		//		chaine de caractere apres le /msg
								char * sender = get_pseudo_from_sock(liste,client_sock);    // pseudo de celui qui envoie
								int length = strlen(message_msg);

								for(k=0;k<length;k++){
									if(stop == 0){
										if(message_msg[k] != ' '){
											oth_pseudo[k] = message_msg[k]; //pseudo du desinataire
											c++;
										}
										else if(message_msg[k] == ' '){ //message à envoyer
											stop =1;
										}
									}
									else if(stop ==1){
										message[k-c-1] = message_msg[k];
									}

								}
								int sock_dest = client_sock_from_pseudo(liste,oth_pseudo);
								printf("sock du destinataire [%d]\n", sock_dest);
								fflush(stdout);
								printf("pseudo du destinataire [%s]\n", oth_pseudo);
								fflush(stdout);
								printf("message a envoyer [%s]\n", message);
								fflush(stdout);

								//envoi du message au destinataire
								memset(buffer,'\0',512);
								sprintf(buffer," [%s]: ", sender);
								strcat(buffer,message);
								strcat(buffer,"\n");
								do_write(sock_dest,server_sock);

								//envoi de confirmation au destinateur
								memset(buffer,'\0',512);
								strcpy(buffer, "[SERVER]: Message sent to user ");
								strcat(buffer, oth_pseudo);
								do_write(client_sock,server_sock);
								memset(buffer,'\0',512);
							}//fin msg

							if(sl_check==7){//create

								msg = get_nick(buffer,sl_check);
								pseudo = get_pseudo_from_sock(liste,client_sock);
								add_canal(c_liste,pseudo,msg,20);
								set_canal_name(liste,pseudo,msg);
								memset(buffer,'\0',512);
								sprintf(buffer," Cannal: %s created",msg);
								do_write(client_sock,server_sock);
							}//fin create

							if(sl_check==8){//join
								ok = 0;
								msg = get_nick(buffer,sl_check);
								pseudo = get_pseudo_from_sock(liste,client_sock);
								ok=join_canal(c_liste,pseudo,msg);
								if (ok==1){
									set_canal_name(liste,pseudo,msg);
									memset(buffer,'\0',512);
									sprintf(buffer," Cannal: '%s' joined",msg);
									do_write(client_sock,server_sock);
								}
								else{
									memset(buffer,'\0',512);
									sprintf(buffer," No Cannal: '%s' found, sorry.",msg);
									do_write(client_sock,server_sock);
								}
							}//fin join

							if (sl_check == 9){ //send file
								char * message_send= get_nick(buffer,sl_check); //path to file
								int length = strlen(message_send);

								for(k=0;k<length;k++){
									if(stop == 0){
										if(message_send[k] != ' '){
											oth_pseudo[k] = message_send[k]; //pseudo du desinataire
											c++;
										}
										else if(message_send[k] == ' '){
											stop =1;
										}
									}


								}
								int client_sock_2 = client_sock_from_pseudo(liste,oth_pseudo);  //numero socket du client 2
								printf("pseudo du destinataire [%s]\n",oth_pseudo);

								fflush(stdout);
								//demande confrimation
								memset(buffer,'\0',512);
								strcpy(buffer ,"[SERVER] : DO you accept [Yes/No] ?");
								do_write(client_sock_2,server_sock);
								memset(buffer,'\0',512);

								//récupérer la reponse du destinataire
								do_read(client_sock_2,server_sock,&sin,1);
								do_write(client_sock,server_sock);

								memset(buffer,'\0',512);
								strcpy(buffer ,"[SERVER] : OK ");
								do_write(client_sock_2,server_sock);

								int port ;
								char* ip_addr= malloc(20*sizeof(char));
								port = get_user_port(liste,client_sock_2);
								ip_addr = get_user_ip_adress(liste,client_sock_2);
								memset(buffer, '\0',512);
								sprintf(buffer ,"%s %d\n",ip_addr,port);
								do_write(client_sock_2,server_sock);
								//do_write(client_sock,server_sock);

								/*printf("port du user2 [%d]\n",port);
								fflush(stdout);
								printf("ip_addr du user 2 [%s]\n",ip_addr);
								fflush(stdout);*/


								//									if(strcmp(buffer,"Yes\n")==0){ //Yes
								//										printf("User 2 wants to receive file");
								//										fflush(stdout);
								//										//envoi de confirmation au destinateur
								//										memset(buffer,'\0',512);
								//										strcpy(buffer,"SEND");
								//										do_write(client_sock,server_sock);
								//										memset(buffer,'\0',512);
								//										//sprintf(buffer," %s  ",path);
								//										//do_write(client_sock,server_sock);
								//
								//
								//
								//									}
								//
								//									else if(strcmp(buffer,"No\n")==0){ //NO
								//										memset(buffer,'\0',512);
								//										strcpy(buffer,"Sorry , User  ");
								//										sprintf(buffer," %s  ", oth_pseudo);
								//										strcat(buffer," refused to accept the file");
								//										do_write(client_sock,server_sock);
								//										memset(buffer,'\0',512);
								//									}




								//envoi du fichier
								//send_file(client_sock,path);




							}




						}// Fin slash
						else if(sl_check ==0){
							if(pseudo_known(liste,client_sock)==1){
								canal_name=get_canal_name_from_sock(liste,client_sock);
								if(!strcmp(canal_name,"global")){
									do_write(client_sock,server_sock);
									memset(buffer,'\0',512);
								}
								else{
									pseudo = get_pseudo_from_sock(liste,client_sock);
									strcpy(tmp,buffer);
									memset(buffer,'\0',512);
									sprintf(buffer," [from %s]: %s",pseudo,tmp);
									speak_with_canal(liste,c_liste,pseudo,canal_name,server_sock);
									memset(buffer,'\0',512);
								}
							}
							else{
								memset(buffer,'\0',512);
								sprintf(buffer,"Veuillez vous connecter pour effectuer cette action");
								do_write(client_sock,server_sock);
								memset(buffer,'\0',512);

							}
						}

					} // Fin du dialogue
				}// Fin d

			} // end of if, gestion evenement


		} //end of for
	}//end of while



	return 0;
}

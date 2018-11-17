#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>


#include "canal.h"
#include "user.h"
#include "server.h"


struct Users {
	int user_sock;
	char* pseudo;
	int connect;
	char* ip_addr;
	int port;
	char* time;
	char* canal_name;
	struct Users *next;
};

struct Liste{
	struct Users *first;
};

struct Users* First_user(struct Liste* liste){
	return liste->first;
}

struct Liste* init(int server_sock){
	struct Liste *liste = malloc(sizeof(struct Liste));
	struct Users *user = malloc(sizeof(struct Users));

	user->user_sock = server_sock;
	user->pseudo = "SERVER";
	user->connect = 1;
	user->ip_addr = NULL;
	user->port = 0;
	user->time = NULL;
	user->canal_name="global";
	user->next = NULL;
	liste->first = user;

	return liste;
}

char* date(){
	time_t secondes;
	struct tm t;
	char* date = malloc(18*sizeof(char));

	time(&secondes);
	t=*localtime(&secondes);
	strftime(date, 18, "%Y/%m/%d@%H:%M ", &t);

	return date;
}

void add_user(struct Liste *liste,char* pseudo, int client_socket){
	//Ajout à la fin de la liste chainé

	struct Users* previous = malloc(sizeof(struct Users));
	struct Users* new = malloc(sizeof(struct Users));

	//	memset(&previous,0,sizeof(previous));
	//	memset(&new,0,sizeof(new));

	new->canal_name = malloc(30*sizeof(char));

	new->user_sock = client_socket;
	new->pseudo = pseudo;
	new->connect = 0;
	new->ip_addr = NULL;
	new->port = 0;
	new->canal_name="global";
	new->time = NULL;
	new->next = NULL;
	previous = liste->first ;
	while(previous->next != NULL)
		previous = previous->next;

	previous->next = new;

}

int del_user(struct Liste *liste,char* pseudo){
	struct Users* previous;
	struct Users* todel;
	struct Users* cur_user;
	int del = 0;
	int stop = 0;
	char* name;

	if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		return 1;
	}

	previous = liste->first;// c'est le serveur
	cur_user = previous->next;

	while(del!=1 && stop!=1){

		if(!strcmp(cur_user->pseudo,pseudo)){
			todel = cur_user;
			name = cur_user->pseudo;
			previous->next = cur_user->next;
			free(todel);
			del = 1;
			fprintf(stdout,"%s a bien été supprimé\n",name);
		}
		previous = cur_user;
		cur_user = cur_user->next;

		if(cur_user == NULL)
			stop=1;
	}

	if( stop != 0)
		fprintf(stdout,"Aucun utilisateurs de ce nom\n");

	return 0;
}


//edit pseudo from pseudo
void edit_pseudo(struct Liste *liste,char* pseudo,char* new_pseudo){
	struct Users* previous;
	struct Users* cur_user;
	int edit = 0;
	int stop = 0;
	char* name;

	if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = liste->first;// c'est le serveur
	cur_user = previous->next;

	while(edit!=1 && stop!=1){

		if(!strcmp(cur_user->pseudo,pseudo)){
			cur_user->pseudo = new_pseudo;
			name = cur_user->pseudo;
			edit = 1;
			fprintf(stdout,"votre nouveau pseudo est bien %s\n",name);
		}
		previous = cur_user;
		cur_user = cur_user->next;

		if(cur_user == NULL)
			stop=1;
	}

}
//edit pseudo from client socket
void edit_pseudo_from_sock(struct Liste *liste,int client_sock,char* new_pseudo){
	struct Users* previous;
	struct Users* cur_user;
	int edit = 0;
	int stop = 0;
	char* name;

	if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = liste->first;// c'est le serveur
	cur_user = previous->next;

	while(edit!=1 && stop!=1){

		if(cur_user->user_sock==client_sock){
			cur_user->pseudo = new_pseudo;
			name = cur_user->pseudo;
			edit = 1;
			//			fprintf(stdout,"votre nouveau pseudo est bien %s\n",name);
			//			fflush(stdout);
		}
		previous = cur_user;
		cur_user = cur_user->next;

		if(cur_user == NULL)
			stop=1;
	}
}

void set_canal_name(struct Liste *liste,char* pseudo,char* canal_name){
	struct Users* previous;
	struct Users* cur_user;
	int edit = 0;
	int stop = 0;

	if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = liste->first;// c'est le serveur
	cur_user = previous->next;

	while((edit!=1) && (cur_user!=NULL)){

		if(!strcmp(cur_user->pseudo,pseudo)){
			cur_user->canal_name=canal_name;
			edit = 1;
			//fprintf(stdout,"Vous avez rejoint le canal: %s",canal_name);
		}
		previous = cur_user;
		cur_user = cur_user->next;

	}

}

char* get_canal_name_from_sock(struct Liste *liste,int client_sock){
	struct Users* previous;
	struct Users* cur_user;
	int edit = 0;
	int stop = 0;


	if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = liste->first;// c'est le serveur
	cur_user = previous->next;

	while(stop!=1){

		if(cur_user->user_sock==client_sock){
			return cur_user->canal_name;

		}
		previous = cur_user;
		cur_user = cur_user->next;

		if(cur_user == NULL)
			stop=1;
	}

	return "\0";
}

//check is pseudo exists
short verify_pseudo(struct Liste *liste,char* pseudo){
	struct Users* previous;
	struct Users* cur_user;
	short* find1 = malloc(sizeof(short));
	*find1 = 0;
	int stop = 0;



	if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = liste->first;// c'est le serveur
	cur_user = previous->next;

	while(*find1!=1 && stop!=1){
		//		printf(" verify_pseudo : %s | %s | %d\n",cur_user->pseudo, pseudo, strcmp(cur_user->pseudo,pseudo));
		//		fflush(stdout);
		if(strcmp(cur_user->pseudo,pseudo)==0){
			*find1 = 1;
			stop = 1;
		}
		previous = cur_user;
		cur_user = cur_user->next;

		if(cur_user == NULL)
			stop=1;
	}

	return *find1;

}


//check if user is already connected
short verify_connect(struct Liste *liste,char* pseudo){
	struct Users* previous;
	struct Users* cur_user;
	short* find1 = malloc(sizeof(short));
	int stop = 0;

	if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = liste->first;// c'est le serveur
	cur_user = previous->next;

	while(*find1!=1 && stop!=1){

		if(!strcmp(cur_user->pseudo,pseudo)){
			if(cur_user->connect == 1){
				*find1 = 1;
			}
		}
		previous = cur_user;
		cur_user = cur_user->next;

		if(cur_user == NULL)
			stop=1;
	}

	return *find1;

}

char* get_pseudo_from_sock(struct Liste *liste,int client_sock){
	struct Users* previous;
	struct Users* cur_user;
	short* find1 = malloc(sizeof(short));
	int stop = 0;
	*find1 = 0;
	char* pseudo= malloc(20*sizeof(char));


	if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = liste->first;// c'est le serveur
	cur_user = previous->next;

	while(*find1!=1 && stop!=1){

		if(cur_user->user_sock == client_sock){
			if(cur_user->pseudo != NULL){
				*find1 = 1;
				pseudo = cur_user->pseudo;

			}
		}
		previous = cur_user;
		cur_user = cur_user->next;

		if(cur_user == NULL)
			stop=1;
	}

	return pseudo;
}


int client_sock_from_pseudo(struct Liste *liste,char* pseudo){
	struct Users* previous;
	struct Users* cur_user;
	int find = 0;
	int stop = 0;
	int sock_dest = 0;

	if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		return 1;
	}

	previous = liste->first;// c'est le serveur
	cur_user = previous->next;

	while(find!=1 && stop!=1){

		if(!strcmp(cur_user->pseudo,pseudo)){
			if(cur_user->user_sock != 0){
				find = 1;
				sock_dest = cur_user->user_sock;
				printf("sock trouvée\n");
				fflush(stdout);
			}
		}

		previous = cur_user;
		cur_user = cur_user->next;

		if(cur_user == NULL)
			stop=1;
	}

	return sock_dest;
}

void down_connect(struct Liste *liste,int client_sock){
	struct Users* previous;
	struct Users* cur_user;
	int find1 = 0;
	int stop = 0;


	if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = liste->first;// c'est le serveur
	cur_user = previous->next;

	while(find1!=1 && stop!=1){

		if(cur_user->user_sock==client_sock && cur_user->connect ==1){
			find1 = 1;
			cur_user->connect = 0;
			fprintf(stdout,"verify :cur_user->co: %d\n",cur_user->connect);
			fflush(stdout);
		}
		previous = cur_user;
		cur_user = cur_user->next;

		if(cur_user == NULL)
			stop=1;
	}

}

//check if a client_sock is affected to a pseudo
short pseudo_known(struct Liste *liste,int client_sock){
	struct Users* previous;
	struct Users* cur_user;
	short* find1 = malloc(sizeof(short));
	int stop = 0;
	*find1 = 0;
	//char* name=NULL;


	if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = liste->first;// c'est le serveur
	cur_user = previous->next;

	while(*find1!=1 && stop!=1){

		if(cur_user->user_sock == client_sock){
			if(cur_user->pseudo != NULL){
				*find1 = 1;
			}
		}
		previous = cur_user;
		cur_user = cur_user->next;

		if(cur_user == NULL)
			stop=1;
	}

	return *find1;
}

void down_client_sock(struct Liste* liste,int client_sock){
	struct Users* previous;
	struct Users* cur_user;
	int edit = 0;
	int stop = 0;


	if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = liste->first;// c'est le serveur
	cur_user = previous->next;

	while(edit!=1 && stop!=1){

		if(cur_user->user_sock==client_sock){
			cur_user->user_sock = 0;
			edit = 1;
		}
		previous = cur_user;
		cur_user = cur_user->next;

		if(cur_user == NULL)
			stop=1;
	}
	fprintf(stdout," Bien deconnecté\n");
	fflush(stdout);
}

void set_info(struct Liste* liste, char* pseudo, int client_sock, struct sockaddr_in c_sin){
	struct Users* previous;
	struct Users* cur_user;
	int find1 = 0 ;
	int stop = 0;


	if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = liste->first;// c'est le serveur
	cur_user = previous->next;

	while(find1!=1 && stop!=1){
		if(!strcmp(cur_user->pseudo,pseudo)){
			find1 = 1;
			cur_user->connect = 1;
			cur_user->ip_addr = inet_ntoa(c_sin.sin_addr);
			cur_user->user_sock = client_sock;
			cur_user->port = ntohs(c_sin.sin_port);
			cur_user->time = date();
			cur_user->canal_name = "global";

			printf("set info :cur_user->ip: %s | cur_user->port: %d | date: %s | cur_user->co: %d | cur_user->user_sock: %d\n",cur_user->ip_addr, cur_user->port, cur_user->time, cur_user->connect, cur_user->user_sock);
			fflush(stdout);
		}
		cur_user = cur_user->next;

		if(cur_user == NULL)
			stop=1;
	}

}

void get_info(struct Liste* liste, char* pseudo, int client_sock,int server_sock){
	struct Users* previous;
	struct Users* cur_user;
	int find1 = 0 ;
	int stop = 0;
	char* tmp = malloc(10*sizeof(char));


	if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = liste->first;// c'est le serveur
	cur_user = previous->next;

	while(find1!=1 && stop!=1){

		if(!strcmp(cur_user->pseudo,pseudo)){
			find1 = 1;
			memset(buffer,0,512);
			strcpy(buffer,pseudo);
			strcat(buffer, " connected since ");
			strcat(buffer,cur_user->time);
			strcat(buffer," with IP adress: ");
			strcat(buffer,cur_user->ip_addr);
			strcat(buffer," and port number: ");
			sprintf(tmp, "%d",cur_user->port);
			strcat(buffer,tmp);
			strcat(buffer,"\n");
			do_write(client_sock,server_sock);
			memset(buffer,0,512);

		}
		previous = cur_user;
		cur_user = cur_user->next;

		if(cur_user == NULL)
			stop=1;
	}
	if(find1 == 0){
		memset(buffer,0,512);
		strcpy(buffer,pseudo);
		strcat(buffer," not found\n");
		do_write(client_sock,server_sock);
		memset(buffer,0,512);
	}
}

void see_user(struct Liste *liste){
	struct Users* cur_user;
	cur_user = liste->first;

	while (cur_user->next != NULL){
		printf(" Pseudo : %s\n",cur_user->pseudo);
		cur_user = cur_user->next;
	}
	printf(" Pseudo : %s\n",cur_user->pseudo);
}


//list of connected users
short see_connected_user(struct Liste *liste, int client_sock,int server_sock, int see_user){
	struct Users* cur_user;
	short* find = malloc(sizeof(short));
	*find = 0;
	cur_user = liste->first;
	while (cur_user->next != NULL){
		if(cur_user->connect == 1){
			if(cur_user->user_sock==client_sock){
				*find = 1;
			}
			if(see_user == 1){
				memset(buffer,'\0',512);
				sprintf(buffer, "            - %s\n",cur_user->pseudo);
				do_write(client_sock,server_sock);
			}
		}
		cur_user = cur_user->next;
	}


	return *find;
}

int nb_of_user(){
	FILE* fich;
	int nb_mot = -1;
	char* l = malloc(sizeof(char));
	int fin = 0;
	fich = fopen("./../src/jalon01/users.txt","r");
	while(fin == 0){
		fread(l,1,1,fich);
		if(l[0] == '\n')
			nb_mot++;
		fin = feof(fich);
	}

	fclose(fich);
	return nb_mot;
}

void init_users(struct Liste* liste,int nb_mot,char** user_liste){
	FILE* fich;
	//int nb_mot = -1;
	char* l = malloc(sizeof(char));
	int fin = 0;
	int ok = 1;
	fich = fopen("./../src/jalon01/users.txt","r");
	if(fich==NULL){
		printf("oups");
	}

	int i=0; //ligne
	int j=0; //colonne
	fin = 0;

	//char mots[nb_mot][28];
	char** mots = malloc(nb_mot*sizeof(char*));
	for(i=0; i<nb_mot;i++){
		mots[i] = malloc(28*sizeof(char));
	}


	fich = fopen("./../src/jalon01/users.txt","r");
	i=0;
	while(fin == 0 && i!=nb_mot){

		fread(l,1,1,fich);
		if(l[0] == '\n'){
			mots[i][j]='\0';
			i++;
			j=0;
		}
		else{
			mots[i][j]=l[0];
			j++;
		}

		fin = feof(fich);
	}

	fclose(fich);
	for(i=0;i<nb_mot;i++){
		strcpy(user_liste[i],mots[i]);
	}

}


void fill_users(struct Liste* liste,int nb_user,char** user_list){

	int i;
	for(i=0;i<nb_user;i++){
		add_user(liste,user_list[i],3);
	}

}

int sock_from_pseudo(struct Liste* liste,char* pseudo){
	struct Users* previous;
	struct Users* cur_user;
	int find1 = 0 ;
	int stop = 0;
	int client_sock=0;

	if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = liste->first;// c'est le serveur
	cur_user = previous->next;

	while(find1!=1 && stop!=1){
		if(!strcmp(cur_user->pseudo,pseudo)){
			find1 = 1;

			client_sock=cur_user->user_sock;
			//			printf("set info :cur_user->ip: %s | cur_user->port: %d | date: %s | cur_user->co: %d | cur_user->user_sock: %d\n",cur_user->ip_addr, cur_user->port, cur_user->time, cur_user->connect, cur_user->user_sock);
			//			fflush(stdout);
		}
		previous = cur_user;
		cur_user = cur_user->next;

		if(cur_user == NULL)
			stop=1;
	}
	return client_sock;
}

//int main(int argc, char** argv){
//
//	struct Liste* liste;
//	int user_nb = nb_of_user();
//	char user_liste[user_nb][28];
//
//	liste=init(3);
//	init_users(liste,user_nb,user_liste);
//	fill_users(liste,user_nb,user_liste);
//
//
//
//	see_user(liste);
//	edit_pseudo(liste,"Theo","Martin");
//
//	verify_pseudo(liste,"Kevin");
//
//
//	char* a=date();
//
//	printf(" %s\n",a);
//
//	return 0;
//
//}

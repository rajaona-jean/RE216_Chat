#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "user.h"

struct Users {
	int user_sock;
	char* pseudo;
	struct Users *next;
};

struct Liste{
	struct Users *first;
};

struct Liste* init(int server_sock){
	struct Liste *liste = malloc(sizeof(liste));
	struct Users *user = malloc(sizeof(user));

	user->user_sock = server_sock;
	user->pseudo = "server";
	user->next = NULL;
	liste->first = user;

	return liste;
}

void add_user(struct Liste *liste,char* pseudo, int client_socket){
	//Ajout à la fin de la liste chainé

	struct Users* previous = malloc(sizeof(previous));
	struct Users* new = malloc(sizeof(new));

	//	memset(&previous,0,sizeof(previous));
	//	memset(&new,0,sizeof(new));


	new->user_sock = client_socket;
	new->pseudo = pseudo;
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

void see_user(struct Liste *liste){
	struct Users* cur_user;
	cur_user = liste->first;

	while (cur_user->next != NULL){
		printf(" Pseudo : %s\n",cur_user->pseudo);
		cur_user = cur_user->next;
	}
	printf(" Pseudo : %s\n",cur_user->pseudo);
}

int nb_of_user(){
	FILE* fich;
	int nb_mot = -1;
	char buffer[1];
	int fin = 0;
	int ok;
	fich = fopen("./users.txt","r");
	while(fin == 0){
		fread(buffer,1,1,fich);
		printf("%s\n",buffer);
		fflush(stdout);
		ok = strcmp(buffer,"\n");
		//		printf("%d\n",ok);
		//		fflush(stdout);
		if(ok == 0)
			nb_mot++;
		fin = feof(fich);
	}

	printf("nb de mot: %d\n",nb_mot);
	fclose(fich);
	return nb_mot;
}

void init_users(struct Liste* liste,int nb_mot,char user_liste[][28]){
	FILE* fich;
	//int nb_mot = -1;
	char buffer[1];
	int fin = 0;
	int ok;
	fich = fopen("./users.txt","r");
	printf("%p\n\n",&fich);
	fflush(stdout);
	if(fich==NULL){
		printf("oups");
	}

	int i=0; //ligne
	int j=0; //colonne
	fin = 0;

	char mots[nb_mot][28];
	memset(&mots,'\0',sizeof(mots)+1);


	fich = fopen("./users.txt","r");
	while(fin == 0){


		fread(buffer,1,1,fich);
		ok = strcmp(buffer,"\n");
		if(ok != 0)
			strcat(mots[i],buffer);
		if(ok == 0){
			strcat(mots[i],"\0");
			printf("%s\n",mots[i]);
			i++;

		}

		fin = feof(fich);
	}

	fclose(fich);
	for(i=0;i<nb_mot;i++){
		strcpy(user_liste[i],mots[i]);
	}

}

void fill_users(struct Liste* liste,int nb_user,char user_list[][28]){

	int i;
	for(i=0;i<nb_user;i++){
		add_user(liste,user_list[i],3);
	}

}
int main(int argc, char** argv){

	struct Liste* liste;
	int user_nb = nb_of_user();
	char user_liste[user_nb][28];

	liste=init(3);
	init_users(liste,user_nb,user_liste);
	fill_users(liste,user_nb,user_liste);
	//   add_user(liste,user,"leo",4);
	//   add_user(liste,user,"kevin",5);
	//   add_user(liste,user,"emilie",6);
	//
	//   see_user(liste,user);
	//   edit_pseudo(liste,user,"emilie","emilio");

	see_user(liste);

	return 0;

}

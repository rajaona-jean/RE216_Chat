#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "canal.h"
#include "user.h"
#include "server.h"

struct Canals{
	char* canal_name;
	int members_nb;
	char** members;
	struct Canals *next;
};

struct First_c{
	struct Canals* first;
};

struct First_c* init_canal(char** user_liste,int user_nb){
	struct Canals *canal = malloc(sizeof(struct Canals));
	struct First_c *c_liste = malloc(sizeof(struct First_c));

	canal->canal_name = "globale";
	canal->members = user_liste;
	canal->members_nb = user_nb;
	canal->next=NULL;
	c_liste->first = canal;

	return c_liste;
}

struct Canals* first_canal(struct First_c* c_liste){
	return c_liste->first;
}

struct Canals* next_canal(struct Canals* canal){
	return canal->next;
}

char* name_canal(struct Canals* canal){
	return canal->canal_name;
}

char* members_canal_i(struct Canals* canal,int i){
	return canal->members[i];
}

void add_canal(struct First_c *c_liste,char* pseudo,char* canal_name, int nb_members){
	//Ajout à la fin de la liste chainé

	struct Canals* previous = malloc(sizeof(struct Canals));
	struct Canals* new = malloc(sizeof(struct Canals));
	int i;
	char** m = malloc(nb_members*sizeof(char*));
	for(i=0; i<nb_members;i++){
		m[i] = malloc(28*sizeof(char));
		m[i]="\0";
	}

	new->members = m;
	new->canal_name = canal_name;
	new->members[0]=pseudo;
	new->members_nb = nb_members;
	new->next = NULL;
	previous =  c_liste->first ; // c'est le canal global
	while(previous->next != NULL)
		previous = previous->next;

	previous->next = new;

}

void del_canal(struct First_c *c_liste,char* pseudo){
	struct Canals* previous = malloc(sizeof(struct Canals));
	struct Canals* todel= malloc(sizeof(struct Canals));
	struct Canals* cur_canal = malloc(sizeof(struct Canals));
	int del = 0;
	int stop = 0;
	char* name;

	if (c_liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas de canal dans la liste\n");
	}

	previous = c_liste->first;// c'est le canal global
	cur_canal = previous->next;

	while(del!=1 && stop!=1){

		if(!strcmp(cur_canal->canal_name,pseudo)){
			todel = cur_canal;
			name = cur_canal->canal_name;
			previous->next = cur_canal->next;
			free(todel);
			del = 1;
			fprintf(stdout,"%s a bien été supprimé\n",name);
		}
		previous = cur_canal;
		if(cur_canal->next == NULL)
			stop=1;
		else
			cur_canal = cur_canal->next;
	}

	if( stop != 0 && del==0)
		fprintf(stdout,"Aucun canal de ce nom\n");

}

void see_canals(struct First_c* c_liste){
	struct Canals* cur_can;
	cur_can = c_liste->first;

	while (cur_can->next != NULL){
		printf(" Pseudo : %s\n",cur_can->canal_name);
		cur_can = cur_can->next;
	}
	printf(" Pseudo : %s\n",cur_can->canal_name);
}

short join_canal(struct First_c* c_liste,char* pseudo, char* canal_name){
	struct Canals* previous;
	struct Canals* cur_canal;
	short* find1 = malloc(sizeof(short)) ;
	*find1=0;
	int i = 0;
	int stop = 0;

	if (c_liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = c_liste->first;// c'est le serveur
	cur_canal = previous->next;

	while(*find1!=1 && (cur_canal!=NULL)){
		if(!strcmp(cur_canal->canal_name,canal_name)){
			*find1 = 1;
			while(stop==0){
				if(cur_canal->members_nb==i){
					stop=1;
				}
				else if(strcmp(cur_canal->members[i],"\0")==0){
					cur_canal->members[i] = pseudo;
					stop=1;
				}
				i++;
			}
		}
		previous = cur_canal;
		cur_canal = cur_canal->next;

	}
	return *find1;
}

void quit_canal(struct First_c* c_liste,char* pseudo, char* canal_name){
	struct Canals* previous;
	struct Canals* cur_canal;
	int find1 = 0 ;
	int stop = 0;
	int i = 0;


	if (c_liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = c_liste->first;// c'est le serveur
	cur_canal = previous->next;

	while(find1!=1 && stop!=1){
		if(!strcmp(cur_canal->canal_name,canal_name)){
			find1 = 1;
			while(stop==0){
				if(strcmp(cur_canal->members[i],pseudo)==0){
					cur_canal->members[i] = "\0";
					stop=1;
					if(i==0){
						del_canal(c_liste,canal_name);
					}
				}
				i++;
			}
		}
		previous = cur_canal;
		cur_canal = cur_canal->next;

		if(cur_canal == NULL)
			stop=1;
	}
}

void see_canal_members(struct First_c* c_liste, char* canal_name){
	struct Canals* previous;
	struct Canals* cur_canal;
	int find1 = 0 ;
	int stop = 0;
	int i=0;


	if (c_liste == NULL){ // si la liste est NULL on s'arrete tout de suite
		printf("error: Pas d'utilisateurs dans la liste\n");
		exit(EXIT_FAILURE);
	}

	previous = c_liste->first;// c'est le serveur
	cur_canal = previous->next;

	while(find1!=1 && stop!=1){
		if(!strcmp(cur_canal->canal_name,canal_name)){
			printf(" %s members:\n",cur_canal->canal_name);
			fflush(stdout);
			find1 = 1;
			while(stop==0){
				if(strcmp(cur_canal->members[i],"\0")!=0){
					printf("       -%s\n",cur_canal->members[i]);
					fflush(stdout);
				}
				if(strcmp(cur_canal->members[i],"\0")==0){
					stop=1;
				}
				i++;
			}
		}
		previous = cur_canal;
		cur_canal = cur_canal->next;

		if(cur_canal == NULL)
			stop=1;
	}
}

//int main(int argc, char** argv){
//
//
//	struct First_c *c_liste = malloc(sizeof(struct First_c));
//	int user_nb = 2;
//	int i;
//	char** user_liste = malloc(user_nb*sizeof(char*));
//	for(i=0; i<user_nb;i++){
//		user_liste[i] = malloc(28*sizeof(char));
//	}
//
//	strcpy(user_liste[0],"Paul");
//	strcpy(user_liste[1], "Kevin");
//
//	c_liste=init_canal(user_liste,user_nb);
//	add_canal(c_liste,"Leo","neo gods",24);
//	see_canals(c_liste);
//	join_canal(c_liste,"Martin","neo gods");
//
//	quit_canal(c_liste,"Leo","neo gods");
//	see_canals(c_liste);
//	free(user_liste);
//	free(c_liste);
//
//	return 0;
//
//}



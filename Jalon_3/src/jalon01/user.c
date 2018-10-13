#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user.h"

struct Users {
  int user_sock;
  char* pseudo;
  struct Users *next;
};

struct Liste{
  struct Users *first;
};

struct Liste* init(int server_sock){
  struct Liste *liste = malloc(sizeof(*liste));
  struct Users *user = malloc(sizeof(*user));

  user->user_sock = server_sock;
  user->pseudo = "server";
  user->next = NULL;
  liste->first = user;

  return liste;
}

void add_user(struct Liste *liste, struct Users *user,char* pseudo, int client_socket){
  //Ajout à la fin de la liste chainé

  struct Users* previous;

  struct Users *new = malloc(sizeof(*new));
  new->user_sock = client_socket;
  new->pseudo = pseudo;

  previous = liste->first ;
  while(previous->next != NULL)
  previous = previous->next;

  previous->next = new;

}

int del_user(struct Liste *liste, struct Users *user,char* pseudo){
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

void edit_pseudo(struct Liste *liste, struct Users *user,char* pseudo,char* new_pseudo){
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

void see_user(struct Liste *liste, struct Users *user){
  struct Users* cur_user;
  cur_user = liste->first;

  while (cur_user->next != NULL){
    printf(" Pseudo : %s\n",cur_user->pseudo);
    cur_user = cur_user->next;
  }
  printf(" Pseudo : %s\n",cur_user->pseudo);
}

void fill_Users(liste,user){
  
}
// int main(int argc, char** argv){
//
//   struct Liste* liste;
//   struct Users* user;
//
//   liste=init(3);
//   add_user(liste,user,"leo",4);
//   add_user(liste,user,"kevin",5);
//   add_user(liste,user,"emilie",6);
//
//   see_user(liste,user);
//   edit_pseudo(liste,user,"emilie","emilio");
//   see_user(liste,user);
//
//   return 0;
//
// }

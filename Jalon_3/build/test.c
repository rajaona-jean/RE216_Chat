#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  struct Users *new = malloc(sizeof(*new));
  new->user_sock = client_socket;
  new->pseudo = pseudo;

  new->next = liste->first;
  liste->first = new;
}

// void del_user(struct Liste *liste, struct Users *user,char* pseudo){
//       struct Users* previous;
//       struct Users* todel;
//
//       if (liste == NULL){ // si la liste est NULL on s'arrete tout de suite
//         printf("error liste NULL\n");
//         return 1;
//       }
//
//       previous = liste;
//
//
//       {
//           struct Users *todel = liste->first;
//           liste->first = liste->first->next;
//           free(aSupprimer);
//       }
// }

void see_user(struct Liste *liste, struct Users *user){
  struct Users* cur_user;
  cur_user = liste->first;

  while (cur_user->next != NULL){
  printf(" Pseudo : %s\n",cur_user->pseudo);
  cur_user = cur_user->next;
}
}

int main(int argc, char** argv){

    struct Liste* liste;
    struct Users* user;

    liste=init(3);
    add_user(liste,user,"leo",4);
    add_user(liste,user,"kevin",5);

    see_user(liste,user);

    return 0;

}

#ifndef USER_H_
#define USER_H_

struct Users;
struct Liste;
struct Liste* init(int server_sock);
void add_user(struct Liste *liste,char* pseudo, int client_socket);
int del_user(struct Liste *liste,char* pseudo);
void edit_pseudo(struct Liste *liste,char* pseudo,char* new_pseudo);
void see_user(struct Liste *liste);
int nb_of_user();
void init_users(struct Liste* liste,int nb_mot,char user_liste[][28]);
void fill_users(struct Liste* liste,int nb_user,char user_list[][28]);

#endif /* USER_H_ */

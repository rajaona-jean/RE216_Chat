#ifndef USER_H_
#define USER_H_

struct Users;
struct Liste;
struct Liste* init(int server_sock);
void add_user(struct Liste *liste, struct Users *user,char* pseudo, int client_socket);
int del_user(struct Liste *liste, struct Users *user,char* pseudo);
void edit_pseudo(struct Liste *liste, struct Users *user,char* pseudo,char* new_pseudo);
void see_user(struct Liste *liste, struct Users *user);

#endif /* USER_H_ */

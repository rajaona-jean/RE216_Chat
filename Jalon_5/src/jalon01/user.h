#ifndef USER_H_
#define USER_H_


struct Users;
struct Liste;
struct Liste* init(int server_sock);
struct Users* First_user(struct Liste* liste);
void add_user(struct Liste *liste,char* pseudo, int client_socket);
int del_user(struct Liste *liste,char* pseudo);
void edit_pseudo(struct Liste *liste,char* pseudo,char* new_pseudo);
void see_user(struct Liste *liste);
int nb_of_user();
void init_users(struct Liste* liste,int nb_mot,char** user_liste);
void fill_users(struct Liste* liste,int nb_user,char** user_list);
short verify_pseudo(struct Liste *liste,char* pseudo);
short verify_connect(struct Liste *liste,char* pseudo);
short pseudo_known(struct Liste *liste,int client_sock);
//get pseudo from socket
char* get_pseudo_from_sock(struct Liste *liste,int client_sock);
//get client_sock from user_sock
char* get_user_ip_adress(struct Liste *liste,int client_sock);

int get_user_port(struct Liste *liste,int client_sock);

int client_sock_from_pseudo(struct Liste *liste,char* pseudo);
void edit_pseudo_from_sock(struct Liste *liste,int client_sock,char* new_pseudo);
void down_client_sock(struct Liste* liste,int client_sock);
short see_connected_user(struct Liste *liste, int client_sock,int server_sock, int see_user);
void down_connect(struct Liste *liste,int client_sock);
void set_info(struct Liste* liste, char* pseudo, int client_sock, struct sockaddr_in c_sin);
void get_info(struct Liste* liste, char* pseudo, int client_sock,int server_sock);
void set_canal_name(struct Liste *liste,char* pseudo,char* canal_name);
char* get_canal_name_from_sock(struct Liste *liste,int client_sock);
int sock_from_pseudo(struct Liste* liste,char* pseudo);

#endif /* USER_H_ */

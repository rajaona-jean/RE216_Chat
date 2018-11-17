#ifndef SEND_H_
#define SEND_H_




struct sockaddr_in init_sender(int port);
int do_socket2();
int do_bind2(int server_sock, struct sockaddr_in sin);
int do_accept2(int server_sock,struct sockaddr_in* c_sin);
char* do_write2(int client_sock,int server_sock);
void send_file(/*int client_sock,*/char* path);

#endif /* SEND_H_ */

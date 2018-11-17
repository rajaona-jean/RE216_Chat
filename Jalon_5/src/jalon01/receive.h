#ifndef RECEIVE_H_
#define RECEIVE_H_




struct sockaddr_in init_sender(int port);
int do_socket();
int do_bind(int server_sock, struct sockaddr_in sin);
int do_accept(int server_sock,struct sockaddr_in* c_sin);
char* do_write2(int client_sock,int server_sock);
void receive_file(char* ip_addr, int port, char* path);

#endif /* RECEIVE_H_ */

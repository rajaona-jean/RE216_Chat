#ifndef CLIENT_H_
#define CLIENT_H_

void error(const char *msg);

int do_socket();
struct sockaddr_in init_server_addr(char* ip_addr,int port,struct sockaddr_in server_sock);
void do_connect(int client_socket,struct sockaddr_in server_sock);

#endif /* CLIENT_H_ */

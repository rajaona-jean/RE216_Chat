#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
char buffer[512];
int N = 4;


/*void ignore( int sig )
{
    fprintf( stderr, "\n"); // This function does nothing except ignore ctrl-c
}*/
void sigintHandler(int sig_num)
{
    signal(SIGINT, sigintHandler);
    printf("\n Cannot be terminated using Ctrl+C \n");
    fflush(stdout);
}


void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int do_socket(){
	int s = socket(AF_INET,SOCK_STREAM,0);
	if (s == -1){
		fprintf(stdout , " serveur: Erreur création de socket\n");
		fflush(stdout);
		error("socket");close(s); exit(EXIT_FAILURE);
	}
	else{
		fprintf(stdout , " serveur: Socket créée %d\n", s);
		fflush(stdout);
	}
	return s;
}



struct sockaddr_in init_serv_addr(int port){
	struct sockaddr_in sin;
	memset(&sin,0,sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	return sin;
}

int do_bind(int server_sock, struct sockaddr_in sin){
	int b = bind(server_sock,(struct sockaddr*)&sin, sizeof(sin));
	if (b == 0){
		printf(" serveur: BIND: OK\n");
		fflush(stdout);
	}
	else{
		printf(" serveur: BIND: ERROR\n");
		fflush(stdout);
		error("bind");close(server_sock); exit(EXIT_FAILURE);
	}
	return b;
}


int do_accept(int server_sock,struct sockaddr_in* c_sin,int q){
	//	socklen_t* c_sin_size =(socklen_t*) sizeof(&c_sin);
	//	printf("%d",c_sin_size);
	//	fflush(stdout);
	int c_sin_size = sizeof(c_sin);
	int client_sock = accept(server_sock,(struct sockaddr*)&c_sin,&c_sin_size);
	if(client_sock == -1){
		error("accept");close(server_sock); exit(EXIT_FAILURE);
	}
	else if(q==0){
		printf(" serveur : client socket: %d\n",client_sock);
		fflush(stdout);
		printf(" serveur: CONECTION : reçu\n");
		fflush(stdout);
	}
	return client_sock;
}

void do_read(int client_sock,int server_sock){
	int txt_size;
	int size_buff = 512;
	txt_size = recv(client_sock,buffer,size_buff,0);
	if(txt_size!=-1){
		printf(" server: MESSAGE FROM CLIENT %d: %s\n",client_sock,buffer);
		fflush(stdout);
	}
	else{
		error("recv");close(server_sock); exit(EXIT_FAILURE);
	}

}

char* do_write(int client_sock,int server_sock){
	char* msg = buffer;
	int size_txt = strlen(msg);
	int s = send(client_sock,msg,size_txt,0);
	if(s!=-1){
		//		printf(" serveur: taille du message: %d \n server: taille du message envoyé: %d\n",size_txt, s);
		//		fflush(stdout);
	}
	else{
		error("send");close(server_sock);exit(EXIT_FAILURE);
	}
	return msg;
}

void clean_up_client_socket(int client_sock,int server_sock, int q){
	int s = close(client_sock);
	if((s!=-1) && (q==0)){
		printf(" server: END OF COMMUNICATION WITH CLIENT %d\n",client_sock);
		fflush(stdout);
	}
	else if ((s!=-1) && (q==1)){

	}
	else if (s == -1){
		error("send");close(server_sock);exit(EXIT_FAILURE);
	}
}

void clean_up_server_socket(int server_sock){
	int s = close(server_sock);
	if(s!=-1){
		printf(" server: CLOSED\n");
		fflush(stdout);
	}
	else{
		error("send");exit(EXIT_FAILURE);
	}
}


int main(int argc, char** argv){
	int server_sock;
	int client_sock;
	struct pollfd fds[N];

	struct sockaddr_in sin = init_serv_addr(atoi(argv[1])); //init the serv_add structure
	struct sockaddr_in c_sin;
	int err; // error
	int k = 0;
	char* msg = malloc(512*sizeof(char));
	int i;
	int j=0;
	int ok = 0;
	int p;

	if (argc != 2)
	{
		fprintf(stderr, "usage: RE216_SERVER port\n");
		return 1;
	}

	//create the socket, check for validity!
	server_sock = do_socket();

	//perform the binding
	//we bind on the tcp port specified
	do_bind(server_sock,sin);


	//specify the socket to be a server socket and listen for at most 20 concurrent client
	err = listen(server_sock,20);
	if(err == 0){
		printf(" serveur: LISTEN: OK\n");
		fflush(stdout);
	}
	else{
		printf(" serveur: LISTEN: ERROR\n");
		fflush(stdout);
		error("listen");close(server_sock); exit(EXIT_FAILURE);
	}

	printf(" serveur: en attente de connections \n\n");
	fflush(stdout);

	//Initis the pollfd structure
	memset(fds, 0,sizeof(fds));
	fds[0].fd = server_sock;
	fds[0].events = POLLIN;
	for(i=0;i<N;i++)
		fds[i].events = POLLIN;

	signal(SIGINT, sigintHandler);

	for (;;)
	{
		while(1){
		memset (buffer, 0, 512);
		p = poll(fds,N,0);
		for(i=0;i<N;i++){
			if(fds[i].revents == POLLIN){
				if((i==0) && (fds[N-1].fd==0)){
					client_sock = do_accept(server_sock,&c_sin,0);
					char* accept="You are connected";
					strcpy(buffer,accept);
					do_write(client_sock,server_sock);
					ok = 0;
					while(ok==0){
						j++;
						if (fds[j].fd == 0){
							fds[j].fd = client_sock ;
							ok = 1;
							j=0;
						}
					}
				}
				else if((i==0) && (fds[N-1].fd!=0)){
					client_sock = do_accept(server_sock,&c_sin,1);
					char* no_accept="Server cannot accept incoming connections anymore. Try again later.";
					strcpy(buffer,no_accept);
					do_write(client_sock,server_sock);
					clean_up_client_socket(client_sock,server_sock,1);
				}

				if(i!=0){
					client_sock = fds[i].fd ;
					//read what the client has to say
					do_read(client_sock,server_sock);
					//we write back to the client
					msg=do_write(client_sock,server_sock);
				}


			}

			if(strcmp(msg,"/quit\n")==0){

				for(i=1;i<N;i++){
					if(fds[i].fd == client_sock)
						fds[i].fd = 0;
				}
				//clean up client socket
				clean_up_client_socket(client_sock,server_sock,0);


			}


		} //end of for
	}



	return 0;
}
}

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
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "canal.h"
#include "user.h"
#include "server.h"
#include "receive.h"
#define MAX_USER 4

char buffer[512];
int N = MAX_USER+1; //on compte le serveur



void receive_file(int client_sock,char* path){

  char dest[512];
  int fin_buffer;

  int i =0,j = 1;
  for(i,j; path[j] != '\0';i++,j++)
        dest[i] = path[j];
    dest[i] = '\0';
    printf("reception fichier : %s\n",dest);
    FILE * Fich = fopen("dest","wb");
    if(Fich != NULL )
  {
      while( fin_buffer == 0) ///dernier octet du buffer
      {
          recv(client_sock, buffer, sizeof( buffer) , 0);
          fwrite(buffer , 1 , strlen(buffer)*sizeof(char) , Fich);
      }
      fclose(Fich);

}
}

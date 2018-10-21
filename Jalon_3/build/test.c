#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	FILE* fich;
	int nb_mot = -1;
	char buffer[1];
	int fin = 0;
	int ok;
	fich = fopen("./users.txt","r");
	printf("%p\n\n",&fich);
	fflush(stdout);
	if(fich==NULL){
		printf("oups");
		return 1;
	}
	while(fin == 0){
		fread(buffer,1,1,fich);
		printf("%s\n",buffer);
		fflush(stdout);
		ok = strcmp(buffer,"\n");
		//		printf("%d\n",ok);
		//		fflush(stdout);
		if(ok == 0)
			nb_mot++;
		fin = feof(fich);
	}

	printf("nb de mot: %d\n",nb_mot);
	fclose(fich);

	char mots[nb_mot][28];
	memset(&mots,'\0',sizeof(mots)+1);
	int i=0; //ligne
	int j=0; //colonne
	fin = 0;

	fich = fopen("./users.txt","r");
	while(fin == 0){


		fread(buffer,1,1,fich);
		ok = strcmp(buffer,"\n");
		if(ok != 0)
			strcat(mots[i],buffer);
		if(ok == 0){
			strcat(mots[i],"\0");
			printf("%s\n",mots[i]);
			i++;

		}

		fin = feof(fich);
	}

	fclose(fich);
	for(i=0;i<nb_mot;i++)
	printf("%s\n",mots[i]);
	return 0;
}

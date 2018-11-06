#ifndef CANAL_H_
#define CANAL_H_


struct Canals;
struct First_c;
struct First_c* init_canal(char** user_liste,int user_nb);
struct Canals* first_canal(struct First_c* c_liste);
struct Canals* next_canal(struct Canals* canal);
char* name_canal(struct Canals* canal);
char* members_canal_i(struct Canals* canal,int i);
void add_canal(struct First_c *liste,char* pseudo,char* canal_name, int nb_members);
void del_canal(struct First_c *liste,char* pseudo);
void see_canals(struct First_c* c_liste);
short join_canal(struct First_c* c_liste,char* pseudo, char* canal_name);
void quit_canal(struct First_c* c_liste,char* pseudo, char* canal_name);
void see_canal_members(struct First_c* c_liste, char* canal_name);

#endif /* CANAL_H_ */

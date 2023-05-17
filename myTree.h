//includo tutte le librerie di cui ho bisogno 
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>

//struct che permette la memorizzazione dei dati di ogni elemento della cartella
//che si sta esplorando
typedef struct{
    char name[200];
    char type;
    char permission[10];
    int size;
    int uid;
    int gid;
    char last_m[20];
    int inodes;
}list;

//struct definita per le opzioni lunghe 
static struct option long_options[] = {
    {"help",    no_argument,        0, 'h'},
    {"inodes",  no_argument,        0, 'i'},
    {"dirsfirst",  no_argument,        0, 'z'},
    {0,         0,                  0,  0 }
};

int option;

//variabili globali per il conteggio delle directory e dei file
int directory = 0, file = 0;

//variabili per definire i flag delle opzioni
int flag_help = 0;
int flag_a = 0;
int flag_d = 0;
int flag_f = 0;
int flag_p = 0;
int flag_s = 0;
int flag_u = 0;
int flag_g = 0;
int flag_D = 0;
int flag_r = 0;
int flag_t = 0;
int flag_L = 0;
int flag_dir = 0;
int flag_inodes = 0;
int flag_error = 0;

//variabile utilizzata per l'opzione -L
//nel caso in cui si voglia definire una profondità massima
int max_depth = INT_MAX;

//intestazione funzioni definite nel programma myTree.c
int numberOfElement(char *path);
void controlOption(int argc, char **argv);
void searchElement(char path[], int depth);
void printElement(list elementi[], char path[], int depth, int n);
void printDepth(int depth);
void flagd(list *vett, int dim);
char* get_permissions(char* path);
void sort(list elementi[], int n);
void swap(list *a, list *b);
void sortForDate(list elementi[], int n);
void sortDir(list elementi[], int n);

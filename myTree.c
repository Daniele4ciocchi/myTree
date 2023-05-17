#include "myTree.h"

void optionHelp(){

    printf("con il comando tree si stampa la lista degli elementi della cartella e sottocartelle\n");
    printf("sintassi: myTree [path] [option]\n");
    printf("---------------------------------------\n");
    printf("[option]\n");
    printf("--help      mostra come utilizzare il comando\n");
    printf("-a          stampa tutto il contenuto della cartella (compresi file nascosti)\n");
    printf("-d          stampa solamente le directory\n");
    printf("-f          stampa il percorso di ogni file/cartella\n");
    printf("-p          stampa i permessi di ogni file/cartella\n");

    flag_help = 1;

    //serie di printf per stampare tutto 
}
//questa funzione controlla l'esistenza di eventuali opzioni al lancio del programma
void controlOption(int argc, char **argv){

    while((option = getopt_long(argc, argv, "hadfpsugDirtzL:", long_options, NULL)) != -1){
        switch(option){
            case 'h':
                optionHelp();
                break;
            case 'a':
                flag_a = 1;
                break;
            case 'd':
                flag_d = 1;
                break;
            case 'f':
                flag_f = 1;
                break;
            case 'p':
                flag_p = 1;
                break;
            case 's':
                flag_s = 1;
                break;
            case 'u':
                flag_u = 1;
                break;
            case 'g':
                flag_g = 1;
                break;
            case 'D':
                flag_D = 1;
                break;
            case 'i':
                flag_inodes = 1;
                break;
            case 'r':
                flag_r = 1;
                break;
            case 't':
                flag_t = 1;
                break;
            case 'z':
                flag_dir = 1;
                break;
            case 'L':
                flag_L = 1;
                max_depth = atoi(optarg);
                max_depth--;
                break;
            default:
                //printf("hai selezionato delle opzioni non esistenti\n");
                flag_error = 1;
        }
    }
}

//funzione che ritorna il numero di elementi all'interno di una cartella 
int numberOfElement(char *path){
    DIR *d = opendir(path);
    struct dirent *dir;
    int n = 0;

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            n++;
        }
    }
    return n;
}

//funzione per la ricerca di elementi all'interno del path
//inoltre inserisce gli elementi all'interno di un vettore 
//di struct per memorizzarne tutti i dati utili
void searchElement(char path[], int depth){

    //variabili per esplorare la cartella
    DIR *d;
    struct dirent *dir;
    struct stat file_stat;
    struct tm* local_time;

    //variabili per sapere il numero di elementi della cartella
    int n = numberOfElement(path);

    //dichiaro un vettore di esattamente n elementi 
    list elementi[n];

    //indice dell'elemento corrente che sto visualizzando
    int index = 0;

    //apro il path per cominciarlo ad esplorare 
    d = opendir(path);

    if (d) {
        while ((dir = readdir(d)) != NULL) {

            //identifico la lunghezza della stringa
            int lunghezza_stringa = strlen(dir->d_name);

            //memorizzazione del tipo di elemento
            switch (dir->d_type) {
                case DT_REG:
                    elementi[index].type = 'F';

                    break;
                case DT_DIR:
                    elementi[index].type = 'D';

                    break;
                default:
                    elementi[index].type = 'F';

            }
            //memorizzazione del nome
            strcpy(elementi[index].name, dir->d_name);

            //costruzione del path appartenente all'elemento corrente 
            //per poter ottenere i dati su di esso
            char path2[1000];
            strcpy(path2,path);
            strcat(path2,"/");
            strcat(path2,elementi[index].name);

            if (flag_p == 1)strcpy(elementi[index].permission,get_permissions(path2));
            if (stat(path2, &file_stat) == 0){
                if (elementi[index].type == 'F')elementi[index].size = file_stat.st_size;
                elementi[index].uid = file_stat.st_uid;
                elementi[index].gid = file_stat.st_gid;
                local_time = localtime(&file_stat.st_mtime);
                strftime(elementi[index].last_m, 20, "%Y-%m-%d %H:%M:%S", local_time);
                elementi[index].inodes = file_stat.st_ino;
            }
            //nel caso di cartelle speciali come ".." o "." viene effettuata la memorizzazione in modo diverso per non cadere in loop involontari
            if ((strcmp(dir->d_name, "..") == 0 && lunghezza_stringa == 2) || (strcmp(dir->d_name, ".") == 0 && lunghezza_stringa == 1)) elementi[index].type = 'P';
            index++;

        }
        closedir(d);
        //nel caso di opzione t eseguo un sort in base alla data di ultima modifica
        if (flag_t == 1)sortForDate(elementi,n);
        //in caso contrario eseguo sempre un sort lessicografico 
        else sort(elementi,n);
        //nel caso in cui le direcotry devono venire prima eseguo un sort 
        if (flag_dir == 1)sortDir(elementi,n);
        //continua ....
        printElement(elementi, path, depth, n);

    }
}

//funzione per stampare la profondità della cartella
void printDepth(int depth){
    for (int i = 0;i<(depth*2);i++)printf("--");
    printf(">");
}

//funzione che crea la stringa dei permessi appartenenti ad un determinato file 
char* get_permissions(char* path) {
    static char permissions[10];

    struct stat st;
    if (stat(path, &st) == -1) {
        return NULL;
    }

    // Trova i permessi del file
    int mode = st.st_mode & 0777;

    // Calcola i permessi per il proprietario, il gruppo e gli altri
    permissions[0] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[3] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[6] = (mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (mode & S_IXOTH) ? 'x' : '-';
    permissions[9] = '\0';

    return permissions;
}

//funzione che stampa gli elementi di una cartella, nel caso in cui 
//l'elemento sia una cartella richiama la funzione searchElement 
//per esplorarla ricorsivamente
void printElement(list elementi[], char path[], int depth, int n){

    for (int i=0; i<n; i++){

        //controllo dei flag per le opzioni
        if (flag_d == 1 && elementi[i].type == 'F' )continue;
        if (flag_a == 0 && elementi[i].name[0] == '.')continue;

        printDepth(depth);

        if (flag_f == 1)printf("%s || ",path);
        if (flag_p == 1)printf("[%s] || ",elementi[i].permission);
        if (flag_u == 1)printf("uid: %d || ",elementi[i].uid);
        if (flag_g == 1)printf("gid: %d || ",elementi[i].gid);
        if (flag_D == 1)printf("ultima modifica: %s || ",elementi[i].last_m);
        if (flag_inodes == 1)printf("inodes: %d || ",elementi[i].inodes);
        if (flag_s == 1 && elementi[i].type == 'F')printf("[%d] || ",elementi[i].size);
        printf("%s",elementi[i].name);
        printf("\n");
        if (elementi[i].type == 'F')file++;

        //nel caso in cui ci siano cartelle le esploro
        if (elementi[i].type == 'P')continue;
        if (elementi[i].type == 'D' ){
            char path2[1000];

            directory++;
            strcpy(path2,path);
            strcat(path2,"/");
            strcat(path2,elementi[i].name);
            if (flag_L == 1 && depth < max_depth)
            searchElement(path2, depth+1);
        }
        
    }
}

//funzione per eseguire il sort in base alla data di ultima modifica
void sortForDate(list elementi[], int n){
    for (int i=0; i<n; i++) {
        for (int j=i+1; j<n; j++) {

            int val = strcmp(elementi[i].last_m, elementi[j].last_m);
            if (val < 0 && flag_r == 0)swap(&elementi[i],&elementi[j]);
            else if (val > 0 && flag_r == 1)swap(&elementi[i],&elementi[j]);
        }
    }
}

//funzione per eseguire il sort lessicografico 
void sort(list elementi[], int n){
    for (int i=0; i<n; i++) {
        for (int j=i+1; j<n; j++) {

            char tmpstr1[200], tmpstr2[200];
            for (int l = 0;l<strlen(elementi[i].name);l++){
                if (elementi[i].name[l] == '.'){
                    tmpstr1[l] = '\0';
                    break;
                }
                tmpstr1[l] = elementi[i].name[l];
            }
            for (int l = 0;l<strlen(elementi[j].name);l++){
                if (elementi[j].name[l] == '.'){
                    tmpstr2[l] = '\0';
                    break;
                }
                tmpstr2[l] = elementi[j].name[l];
            }


            int val = strcmp(tmpstr1, tmpstr2);
            if (val > 0 && flag_r == 0)swap(&elementi[i],&elementi[j]);
            else if (val < 0 && flag_r == 1)swap(&elementi[i],&elementi[j]);
        }
    }
}

//funzione per ordinare gli elementi in modo tale che le cartelle
// vengano prima 
void sortDir(list elementi[], int n){
    for (int i=0; i<n; i++) {
        for (int j=i+1; j<n; j++) {
            if (elementi[i].type == 'F' && elementi[j].type == 'D')swap(&elementi[i],&elementi[j]);
        }
    }
}

//funzione per eseguire lo scambio di due elementi all'interno di un vettore
void swap(list *a, list *b) {
    list temp = *a;
    *a = *b;
    *b = temp;
}

int main (int argc, char **argv){
    //controllo le opzioni
    char *path = ".";
    if (argv[1] && argv[1] != "-")path = argv[1];

    controlOption(argc, argv);
    if (flag_help == 1 || flag_error == 1) return 0;
    searchElement(path,0);
    
    printf("%d directory, %d file\n", directory, file);

}
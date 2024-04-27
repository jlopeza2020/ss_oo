
enum {
    LineSz = 512,
};

int debug;

enum {
    Hkind, 
    Ckind,
    Nkind, // para saber la cantidad de tipos que tengo

}; // 

char  *extensions[Nkind] = {}
struct Cfilectrs{
    int kind[Nkinds];
    long long size; 
};
typedef struct Cfilectrs Cfilectrs;

int 
mustignore(char *s){
    return strcmp(s, ".") == 0 || strcmp(s, "..") == 0;
}
int 
isfile(struct stat *st){
    return (st->st_mode & S_IFMT)
}
int 
isdir(struct stat *st){
    return(st->st_mode & )
}
int 
kindnext
{ 
    int kind; 

    kind = -1;
    for(i = 0; i < Nkind; i++)

        if(strcmp(ext, extensions[i]) == 0){
            return i;
        }

    //return kind;
}
void 
countcfiles{
    
    ext = strrchr(path, '.');

    if8ext == NULL
    kind = kindnext(ext);

}
char *
makepath(char *d, char *f){

    strlen(d) + strlen(f) +2
    malloc 

}

int 
scancfiles{
    DIR *d;
    struct dirent 

    ret = 0;
    if(debug){
        scanning path
    }

    d = opendir(path);
    if ( d == NULL)

    current = NULL;
    while ((ent = readdir(d)) != NULL){

        if(mustignore( ent->d_name)){
            continue;
        }
        current = makepath(path, ent->d_name);
        if(stat(current, &st) < 0) // d_dtype no es portable y hay sistemas de ficheros que no rellenan ese campo 
            ret = -1
        if(isdir(&st) && scancfiles(current, ctrs) < 0){
            ret = -1
            break;

        }else if(isfile(&st)){
            countcfiles(current, &st, ctrs);
        }
        free(current);
        current = NULL; // importante: porque se hace un segundo free pq free de NULL es inocuo
    }
    free(current);
    closedir(d);
    return ret;
}
int 
main(int argc, char *argv[]){ // Se agregaron los argumentos que faltaban
    
    char line[LineSz];
    int errs; 
    Cfilectrs ctrs;

    // uso justificado de una variable global
    if(argc == 2 && strcmp(argv[1], "-d") == 0){
        debug++; 
        argv++;
        argc--;
    }
    if(argc != 1)
        usage

    errs = 0;
    while (fgets(line, LineSz, stdin) != NULL){
        if(line[strlen(line) -1] != '\n'){ // aplicar esta modificacione a la shell 
            warnx("line too long");
            continue;
        }
        
        line[strlen(line) -1] = '\0';
        memset(&ctrs, 0, sizeof(Cfilectrs));
        if(scancfiles(line,  &ctrs) < 0){
            warnx("errors scanning %s", line)
            errs++;
            continue;
        }
        // printctrs(ctrs);
        printf()
    }
    if(ferror(stdin)){
        errx(EXIT_FAILURE, "read error");
    }

    exit(errs); 
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>

// enum MaxBuf
enum 

struct Transfer{
    long long count 
    long long bufsize
    int fdin;
    int fdout;

}
typedef struct Transfer Transfer;

void 
closetransfer(Transfer *t) {
    if (t->fdin != 0) {
        close(t->fdin);
    }
    if (t->fdout != 1) {
        close(t->fdout);
    }
}

void 
usage(void){
    fprintf(stderr, "usage: copybytes in out buffersize [count]\n");
    exit(EXIT_FAILURE);
}

long long
dotransfer(Transfer *t)
{
    int toread;
    int done;
    long long total;
    char *buf;

    buf = (char *) malloc(t->bufsize);
    if(buff == NULL){
        err(EXIT_FAILURE, "out of memory");
    }
    // comprobar null

    total = 0;
    done = 0;

    while(!done){
        toread = t->bufsize;
        if(t-> count != -1 && t-> count -total < t->buffsize){
            toread = t->count - total;
        }
        nr = read(t->fdin,buf,toread); // toread saber los que quiero leer
        if(nr < 0)
        {
            err(EXIT_FAILURE, "read error");
            break;
        }
        if(nr == 0) //final fichero 
            break;

        if(write(t->fdout, buf, nr)!= nr){
            break;
        }
        total += nr;
        done = t-> count != -1 && t-> count == total;

    }
    free(buf);
    return total;
}
long long 
parsell(char *str){
    long long ll;
    char *endprt;

    errno = 0;
    ll = strtoll(str, &endptr,10);
    if(errno != 0 || endptr - str != strlen(str)){
        usage();
    }
    return ll;
}
void 
gettransfer(){
    t -> fdin = 0;
    t -> fout = 1;
    t -> count = -1

    if(ntargs == 4){
        t-> count = parsell(targs[3]);
        if(t->count < 0){
            errx(EXIT_FAILURE, "Invalid count value");
        }
        ntargs--;
    }
    if(ntargs != 3){
        usage();
    }
    t -> bufsize = parsell(targs[2]);
    if(t-> bufsize < 1 || t-> bufsize > bufsize)
        errx(EXIT_FAILURE, "Invalid buffer size"); 

    if (strcmp(targs[0], "-") != 0) {
        t->fdin = open(targs[0], O_RDONLY);
        if (t->fdin == -1) {
            err(EXIT_FAILURE, "Cannot open input file");
        }
    }
    if (strcmp(targs[1], "-") != 0) {
        t->fdout = open(targs[1], O_WRONLY | O_CREAT | O_TRUNC, 0664);
        if (t->fdout == -1) {
            err(EXIT_FAILURE, "Cannot open output file");
        }
    }

}

int 
main(){
    Transfer t;
    if (argc < 4 || argc > 5) {
        usage();
    }

    gettransfer(&t, argc - 1, argv + 1);

    if (dotransfer(&t) < 0) {
        err(EXIT_FAILURE, "Error in transferring bytes");
    }
    closetransfer(&t);

    exit(EXIT_SUCESS);

}
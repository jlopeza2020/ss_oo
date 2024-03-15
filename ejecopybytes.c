

// enum MaxBuf

struct Transfer
// 
long long count 
long long bufsize
int fdin;
int fdout;

closetransfer{
    if(t -> fdin != 0)
    {
        close(t->fin)
    }
    // igula con fout
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
        if(nr <0)
        {
            error
            break
        }
        if(nr == 0) //final fichero 

        if(write(t->fdout, buf, nr )!= nr){
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
            errx()
        }
        ntargs--;
    }
    if(ntargs != 3){
        usage();
    }
    t -> bufsize = parsell(targs[2]);
    if(t-> bufsize < 1 || t-> bufsize > bufsize)
        // error 
    if(strcmp(targs[0], "-") )
        t->fdin = open(targs[0], O_RDONLY)
        //COMPROBAR ERROR 
    IF(  // SALIDA
        // 0664
    )

}

int 
main(){
    Transfer t; 
    gettransfer
    if(dotransfer(&t) < 0){
        errx(EXIT_FAILURE, "    ")
    }
    closetransfer(&t)

}
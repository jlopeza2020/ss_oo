

int ps(){
    if(pipe())

    swithc(fork()){
        case -1;

        case 0: 
        // operaciones que hace el hijo 
        close (p[0]);
        dup2(p[1],1);
        close(p[1]);
        execl("/bin/ps", "ps", "-eo", "%p %u %x %c", NULL);
        err()
    }
    close(p[1]);

    return pid;
}

long str2pid{
    l = strtol

    return l;
}
long 
getlinepid()
{
    strtok
}
int mustprint
{
    long pid;
    pid=getlinepid(l);
    return pid == -1 || pid>= min && pid <= max
}
int 
listprocs(){
    FILE *f;

    f= fdopen(ps(), "r");

    while(fgets){
        if(mustprint(line, min, max)){
            print(line)
        }
    }

    if(ferror(f)){
        fclose(f);
        return -1;
    }

    fclose(f);

    while(wait(&sts))
}

int 
main(){

    long maxpid; 
    long minpid;

    if(argc != 3)
    {
        usage
    }

    if(minpid = str2pid() == -1)
    if(maxpid = str2pid() == -1)

    if(min>= max){

    }
    listprocs(long min)


}
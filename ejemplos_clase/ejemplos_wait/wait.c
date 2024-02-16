


enum{
    Nchildren = 5;
}

int 
main(int argc, char *argv[]){

    int pid;
    int sts;
    int i; 

    for(i = 0; i<Nchildren; i++){
        pid = fork();

        switch(pid){
            case -1:
                err("fork fialed")
            case 0:
                execl("/bin/sleep/", "sleep", "10", NULL);
                err
            default:
    }
}

while((pid = wait(&sts)) !=1){ 



}
// a veces cambia su estado pero no muere, tener cuidado
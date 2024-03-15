void 
ping(char *addr)
{
    switch(fork()){
    case -1:
    // error 
    case 0:
        execl("/bin/ping", "myping", "-c1", "-W5", ip, NULL);
		err(EXIT_FAILURE, "exec failed");
    }
}

int 
main(){

    int i;
    int pid; 
    int sts;
    int errs = 0;

    if(argc < 2){
        errx(1, "usage :%s ip ... ", argv[0]);
    }
    for(i = 1; i< argc; i++){
        ping(argv[i]);
    }
    while((pid = wait(&sts)) != -1){
        if(WIEXITED(sts) && WEXITSTATUS(sts) != 0){
            fprintf(stderr, "pid %d exited, status: %d\n",
                pid, WEXITSTATUS(sts));
            errs++;
        }else if(WIFSIGNALED(sts)){
            err++;
        }
    }
    exit(errs);
}
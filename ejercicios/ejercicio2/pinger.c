#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
enum {

	NoArgs,		
};

void
usage(void)
{
	fprintf(stderr,
		"usage: pinger ip/name [ip/name ...]\n");
	exit(EXIT_FAILURE);
}


int
main(int argc, char *argv[])
{
	int pid;
    int sts;
    int i; 
    
    // Omitimos primer argumento
	argc--;
	argv++;

    if(argc == NoArgs){
        usage();
    }

    for(i = 0; i < argc; i++){

        pid = fork();

        switch(pid){
            case -1:
                err(EXIT_FAILURE, "fork failed!") ;
            case 0:
                execl("/bin/sleep", "sleep", "10", NULL);
                err(EXIT_FAILURE , "exec failed") ;
            default:
                printf("child created : %d\n", pid ) ;
        }
    }

    while((pid = wait(&sts)) != -1){ 
        printf("Did process %d exit?\n", pid);

        if(WIFEXITED(sts)) {
            printf("Yes! the status was : %d\n" , WEXITSTATUS(sts) ) ;
        } else {
            printf("No\n");
        }
    }

	exit(EXIT_SUCCESS);
}

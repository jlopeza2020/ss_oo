#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <err.h>

enum {
    THREE = 3,
	READ_END = 0,
	WRITE_END = 1,
};

struct Command
{
    int times;
    char **args;
	char *new_path1;
	char *new_path2;
};

void
usage(char *program_name)
{
	fprintf(stderr, "usage: %s : 'COMMAND' 'COMMAND' 'COMMAND' \n", program_name);
	exit(EXIT_FAILURE);
}

void
token_arg(char *cmd, char **wrds_exec)
{
	char *token;
	char *rest = cmd;
	int i = 0;

	while ((token = strtok_r(rest, " ", &rest))) {
		wrds_exec[i] = token;
		i++;
	}
}

void 
starting_fork(int fd_read, int fd_write, struct Command command)
{
	int pid;
	pid = fork();
	
    switch (pid){
        case -1:
			free(command.new_path1);
			free(command.new_path2);
         	err(EXIT_FAILURE , "cannot fork");
			break;

        case 0:
           	close(fd_read);
          	if(dup2(fd_write, STDOUT_FILENO) < 0) {
				free(command.new_path1);
				free(command.new_path2);
              	err(EXIT_FAILURE , "dup failed");
          	}

    		close(fd_write);
	
          	execv(command.new_path1,command.args);
			execv(command.new_path2,command.args);
			free(command.new_path1);
			free(command.new_path2);
			err(EXIT_FAILURE , "exec failed");
			break;

		default: //father
			close(fd_write);
	}
}

void 
middle_fork(int fd1_read, int fd2_read, int fd2_write,struct Command command)
{
	int pid;
	pid = fork();
	
    switch (pid){
        case -1:
			free(command.new_path1);
			free(command.new_path2);
         	err(EXIT_FAILURE , "cannot fork");
			break;
        case 0:

			close(fd2_read); 

            if(dup2(fd1_read, STDIN_FILENO) < 0) {
				free(command.new_path1);
				free(command.new_path2);
              	err(EXIT_FAILURE , "dup failed");
          	}
	        close(fd1_read);

			if(dup2(fd2_write, STDOUT_FILENO) < 0) {
				free(command.new_path1);
				free(command.new_path2);
              	err(EXIT_FAILURE , "dup failed");
          	}		
	        close(fd2_write);

			execv(command.new_path1,command.args);
			execv(command.new_path2,command.args);
			free(command.new_path1);
			free(command.new_path2);
			err(EXIT_FAILURE , "exec failed");
			break;

		default: //father

			close(fd1_read); 
            close(fd2_write); 
	}
}

void
end_fork(int fd_read, int fd_write, struct Command command)
{
	int pid;

	pid = fork();
	
    switch (pid){
        case -1:
			free(command.new_path1);
			free(command.new_path2);
         	err(EXIT_FAILURE , "cannot fork");
			break;
        case 0:
          	if(dup2(fd_read, STDIN_FILENO) < 0) {
				free(command.new_path1);
				free(command.new_path2);
              	err(EXIT_FAILURE , "dup failed");
          	}
    		close(fd_read);
			execv(command.new_path1,command.args);			
			execv(command.new_path2,command.args);
	
			free(command.new_path1);
			free(command.new_path2);
			err(EXIT_FAILURE , "exec failed");
			break;
	}
}

void
process_output(int num_params, char **params)
{
	int pos, lon_palabra, status, this_pos, following_pos, fds[num_params][2];

    struct Command *commands;
	int length_first_arg, total_length1, total_length2,length_static_path1, length_static_path2;
	char *path1, *path2;
    commands = malloc(sizeof(struct Command) * num_params);
	
	this_pos = 0;
	following_pos = 1;
	for(pos = 0;pos < num_params; pos++)
    { 
    
		lon_palabra = strlen(params[pos + 1]);
        commands[pos].args = malloc((lon_palabra + 1) * sizeof(char *));

        token_arg(params[pos + 1],commands[pos].args);

		path1 = "/bin/";
		path2 = "/usr/bin";

		length_first_arg = strlen(commands[pos].args[0]);
		length_static_path1 = strlen(path1);
		length_static_path2 = strlen(path2);
		total_length1 = length_first_arg + length_static_path1;
		total_length2 = length_first_arg + length_static_path2;
		commands[pos].new_path1 = malloc(sizeof(char) * (total_length1 + 1));
		commands[pos].new_path2 = malloc(sizeof(char) * (total_length2 + 1));
		strcat(strcpy(commands[pos].new_path1, path1),commands[pos].args[0]);
		strcat(strcpy(commands[pos].new_path2, path2),commands[pos].args[0]);

		if(pos == 0)
		{
			if(pipe(fds[0]) < 0)
        		err(EXIT_FAILURE , "cannot make a pipe");

			starting_fork(fds[0][READ_END], fds[0][WRITE_END], commands[pos]);
		}
		else if(pos == num_params - 1)
		{
			end_fork(fds[num_params-2][READ_END], fds[num_params-2][WRITE_END], commands[pos]);
			close(fds[num_params-2][READ_END]);
		}
		else
		{
			
			if(pipe(fds[following_pos]) < 0)
        		err(EXIT_FAILURE , "cannot make a pipe");

			middle_fork(fds[this_pos][READ_END], fds[following_pos][READ_END], fds[following_pos][WRITE_END], commands[pos]);
			this_pos++;
			following_pos++;
		}
    }

	int p;
	for(p = 0; p < num_params; p++)
    {
		wait(&status);
		free(commands[p].args);
		free(commands[p].new_path1);
		free(commands[p].new_path2);
    }
	free(commands);
}

int
main(int argc, char *argv[])
{
	int parameters;

	parameters = argc - 1;

	if (parameters >= 2) {
		process_output(parameters, argv);
		exit(EXIT_SUCCESS);
	} else {
		usage(argv[0]);
	}

}
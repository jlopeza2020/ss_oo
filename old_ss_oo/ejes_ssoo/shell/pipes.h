
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
usage(char *program_name);

void
token_arg(char *cmd, char **wrds_exec);

void 
starting_fork(int fd_read, int fd_write, struct Command command);

void 
middle_fork(int fd1_read, int fd2_read, int fd2_write,struct Command command);

void
end_fork(int fd_read, int fd_write, struct Command command);

void
process_output(int num_params, char **params);
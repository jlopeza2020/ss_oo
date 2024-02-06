
enum {
	MAX = 1024,
};

int 
equal_strings(char string1[MAX],char string2[MAX]);
void 
command(char string[MAX], int plano, int parameters, char **params);
void 
stdin_red(char string[MAX]);
void 
stdout_red(char string[MAX]);
void 
pipes(char* arg1[MAX], char* arg2[MAX]);
void 
create_process(char* arg[MAX], int plano);
void
token_args(char *cmd, char **wrds_exec);
void
built_slay(char *num , int plano);
void 
token_spaces(char *cmd, char **wrds_exec);
long
is_lnguint(char **args_values, int pos);


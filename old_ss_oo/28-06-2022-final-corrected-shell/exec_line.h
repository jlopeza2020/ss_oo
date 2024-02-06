
enum {
	MAX = 1024,
};

int 
equal_strings(char *string1,char *string2);

void 
process_output(char *string, int bg, int times_executed);

void 
stdin_red(char *string);

void 
stdout_red(char *string);

int 
get_positions(char *cmd, char **wrds_exec);

void
cd_builtin(char *string);

void
set_value(char *string);

int
sustitutions(char *string);
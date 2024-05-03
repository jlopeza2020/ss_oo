
enum {
	Cd, 
	NbuiltIn,
};

void findcommands(CommandLine *cl);
void findtypecommand(CommandLine *cl,char *cmd);
int isbuiltin(char *cmd);
int ispwd(char *cmd);
int ispath(char *cmd);
long long getnumpathtokens(char *line);
char **tokenpath(char *path, long long times);
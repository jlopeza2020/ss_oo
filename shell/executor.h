
enum {
	cd, 
	NbuiltIn,
};

// FIND	EXECUTOR
void findcommands(CommandLine *cl);
void findtypecommand(CommandLine *cl,char *cmd, int *statusbt);
int isbuiltin(char *cmd, int *status);
int setpwd(char *cmd);
int setpath(char *cmd);
long long getnumpathtokens(char *line);
char **tokenpath(char *path, long long times);

// EXECUTOR
// setbuiltin
void executecommands(CommandLine *cl);
pid_t executecommand(CommandLine *cl, char ***comandline, long long *numwords, long long *pos);
void executebuiltin(CommandLine *cl, char **comandline, int type, long long numwords);
void executecd(char **cl, long long numwords);

// redirections 
void handleredirecctions(CommandLine *cl);
void openredout(CommandLine *cl);
void openredin(CommandLine *cl);

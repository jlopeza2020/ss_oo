
enum {
	cd,
	ifok,
	ifnot,
	NbuiltIn,
};

// FIND EXECUTOR
void findcommands(CommandLine *cl);
void findtypecommand(CommandLine *cl, char *cmd, int *statusbt);
int isbuiltin(char *cmd, int *status);
int setpwd(char *cmd);
int setpath(char *cmd);
long long getnumpathtokens(char *line);
char **tokenpath(char *path, long long times);

// EXECUTOR
// builtins
void executecommands(CommandLine *cl);
pid_t executecommand(CommandLine *cl, char ***comandline, long long *numwords,
		     long long *pos, int typebuiltin);
void executebuiltin(CommandLine *cl, char **comandline, int type,
		    long long numwords);
void executecd(char **cl, long long numwords);

// redirections 
void handleredirecctions(CommandLine *cl);
void openredout(CommandLine *cl);
void openredin(CommandLine *cl);

// backgroud 
void setbg(CommandLine *cl);

// wait
void setwait(pid_t * waitpids, long long childs);

//OPCIONAL 1
void sethere(int *herepipe);

//OPCIONAL 2
long long getnumber(char *str);
void execifcommand(CommandLine *cl);

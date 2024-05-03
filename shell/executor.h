
enum {
	Cd, 
	NbuiltIn,
};

void findcommands(CommandLine *cl);
void findtypecommand(CommandLine *cl,char *str);
int isbuiltin(char *str);
int ispwd(char *str);
int ispath(char *str);

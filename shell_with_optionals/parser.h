
long long getnumwords(char *line);
void handlespecialchars(CommandLine *cl, char *word, long long *pos);
void tokenize(CommandLine *cl, char *line);
void freememory(CommandLine *cl);


void parse(CommandLine *cl);

// $
void caseenv(CommandLine *cl);
void setenvvar(CommandLine *cl, char *Str);
int isenv(char *str);
void elimfirstchar(char *word);

// &
void casebg(CommandLine * cl);
int isbg(CommandLine *cl);

// > < 
void casered(CommandLine *cl);
void elimstr(CommandLine * cl, long long pos);
int isred(CommandLine *cl, char *typered);
int isambiguoischar(char letter);
int isname(char *word);
int isstr(char *word);
void setred2struct(CommandLine *cl, char *file, int value, int status);

// |
void casepipes(CommandLine * cl);
void checkpipessyntax(CommandLine *cl);
void handlepipes(CommandLine *cl);
void setnumcommnads(CommandLine *cl);
void setcommands(CommandLine *cl);

// =: se considerará cuando no haya pipes
void caseequal(CommandLine *cl);
void setequal(char *str);
int isequal(char *str);


//OPCIONAL 1
void casehere(CommandLine *cl);

//OPCIONAL 3
void caseglob(CommandLine *cl);
void setglobbing(CommandLine *cl, char *token, long long *pos);
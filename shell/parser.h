
// manejar errores parsing
enum {
    PARSINGERROR = -1,
    INPUTRED = 2,
    OUTPUTRED,
    BOTHRED = 5,
};

int getnumwords(char *line);
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
void handlered(CommandLine *cl, char *file, int value, int status);

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

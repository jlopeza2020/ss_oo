
enum {
	MaxWord = 512,
};

enum {
	   // todos los strcmp
    PIPE,
    STDINRED,
    STDOUTRED,
    BACKGROUND,
    ENV,
    EQUAL,
    BUILTINCD,
    WORD,
};

struct CommandLine{
	char **words;
    int numwords;
    int bg;
    int stdired;
    int stdored;
	int numpipes;
    int env;
    int equal;
};
typedef struct CommandLine CommandLine;

// otra estructura para comando 
// otra para pipes 
// asigne los descriptores de fichero ... 
// mire si hay builtin...

// ALMACENARLO EN OTRO .H
struct Redirections{
    char *stdinred;
    int fdstdinred;
    char *stdoutred;
    int fdstdoutred;
};
typedef struct Redirections Redirections; 


int getnumwords(char *line);
void tokenize(CommandLine *cl, char *line);
void freememory(CommandLine *cl);
void parse(CommandLine *cl);

//int gettype(char *str, int actualpos, int totalpos);
//void settype(CommandLine *cl, int val);

//int isbg(char *str, int actualpos, int totalpos);
int isbg(CommandLine *cl);
int isred(CommandLine *cl, char *typered);
int isstr(char *word);

int isenv(char *str);
int isequal(char *str);

void casebg(CommandLine * cl);
void casered(CommandLine *cl);
void elimstr(CommandLine * cl, int pos);


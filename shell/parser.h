
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

// manejar errores parsing
// en concreto ahora con los errores de redirecciones
enum {
    PARSINGERROR = 1,
    INPUTRED,
    OUTPUTRED,
    BOTHRED = 5,
};

struct CommandLine{
	char **words;
    int numwords;
    int bg;
    int stdired;
    char *inred;
    int stdored;
    char *outred;
	int numpipes;
    int env;
    int equal;
    int status;
};
typedef struct CommandLine CommandLine;

// otra estructura para comando 
// otra para pipes 
// asigne los descriptores de fichero ... 
// mire si hay builtin...

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


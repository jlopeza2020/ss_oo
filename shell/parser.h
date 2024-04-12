//#include "structs.h"

enum {
	MaxWord = 4*1024, // 4k  
};

// manejar errores parsing
// en concreto ahora con los errores de redirecciones
enum {
    PARSINGERROR = 1,
    INPUTRED,
    OUTPUTRED,
    BOTHRED = 5,
};

/*struct CommandLine{

    char **words; // eje: [cat, /tmp/a, |, wc -c, >, x, <, y, &]
    int numwords; // eje: 10
    char ***commands; // eje: [[cat, /tmp/a], [wc, -c]]
    int *numsubcommands; // eje: [2,2]
    int numcommands; // eje: 2
    int bg;
    int stdired;
    char *inred; // eje: y
    int stdored;
    char *outred; // eje: x
	int numpipes;
    int env;
    int equal;
    int status;
};
typedef struct CommandLine CommandLine;*/

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

//int isenv(char *str);
//int isequal(char *str);

void casebg(CommandLine * cl);
void casered(CommandLine *cl);
void elimstr(CommandLine * cl, int pos);
void handlered(CommandLine *cl, char *file, int value, int status);

void casepipes(CommandLine * cl);
void handlepipes(CommandLine *cl);
void setnumcommnads(CommandLine *cl);
void setcommands(CommandLine *cl);

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

struct CheckInput{
	char **words;
    int numwords;
	/*int numpipes;
    int stdired;
    int stdored;
    int bg;
    int env;
    int equal;*/
};
typedef struct CheckInput CheckInput;

// otra estructura para comando 
// otra para pipes 
// asigne los descriptores de fichero ... 
// mire si hay builtin...

int getnumwords(char *line);
void tokenize(CheckInput *checkinput, char *line);
void freememory(CheckInput *checkinput);
void parse(CheckInput *checkinput);
int gettype(char *str, int actualpos, int totalpos);
void settype(CheckInput *checkinput, int val);
int isbg(char *str, int actualpos, int totalpos);
int isenv(char *str);


enum {
	MaxWord = 256,
};

struct CheckInput{
	char **words;
    /*int numwords;
	int numpipes;
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
void tokenize(CheckInput *checkinput, char *line, int numwords);
void freememory(CheckInput *checkinput, int numwords);

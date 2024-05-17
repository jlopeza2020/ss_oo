enum {
	MaxWords = 1024,
    MaxWord = 4*1024, // 4k  
};

enum {
    REDERROR = -3,
    PARSINGERROR,
    FINDERROR,
};

enum {
    INPUTRED = 2,
    OUTPUTRED,
    BOTHRED = 5,
};
enum {
    READ = 0,
    WRITE = 1,
};
struct CommandLine{

    char **words; // eje: [cat, /tmp/a, |, wc -c, >, x, <, y, &]
    long long numwords; // eje: 10
    char ***commands; // eje: [[cat, /tmp/a], [wc, -c]]
    long long *numsubcommands; // eje: [2,2]
    long long numcommands; // eje: 2
    int bg;
    int inrednum;
    int inredfd; // eje: decriptor de fichero creado para y
    char *inred; // eje: y
    int outrednum;
    int outredfd; // eje: descriptor de fichero creado para x
    char *outred; // eje: x
	int numpipes;
    int **pipesfd;
    int status;
    int statusred;
    // status del comando si  es un builtin sin pipes 
    int statusbt; // inicializada a -1
    // status de cada comando si hay pipes y son builtins
    int *statuspipesbt; // eje: [-1, -1] al ser ningún comando un builtin
};
typedef struct CommandLine CommandLine;
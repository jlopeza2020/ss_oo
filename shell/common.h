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
    //PARSINGERROR = -1,
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
    int inredfd;
    char *inred; // eje: y
    int outrednum;
    int outredfd;
    char *outred; // eje: x
	int numpipes;
    int **pipesfd;
    int status;
    int statusred; // si es error, si hay redirección entrada, salida o las 2
    // status del builtin si solo hay commandos sin pipes
    int statusbt; // inicializada a -1
    // status de cadad comando si hay pipes y son builtins
    int *statuspipesbt;
};
typedef struct CommandLine CommandLine;
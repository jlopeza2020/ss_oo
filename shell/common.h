enum {
	MaxWords = 1024,
    MaxWord = 4*1024, // 4k  
};

enum {
    PARSINGERROR = -2,
    FINDERROR = -1,
};

struct CommandLine{

    char **words; // eje: [cat, /tmp/a, |, wc -c, >, x, <, y, &]
    long long numwords; // eje: 10
    char ***commands; // eje: [[cat, /tmp/a], [wc, -c]]
    long long *numsubcommands; // eje: [2,2]
    long long numcommands; // eje: 2
    int bg;
    int stdired;
    char *inred; // eje: y
    int stdored;
    char *outred; // eje: x
	int numpipes;
    int status;
    int statusred; // si es error, si hay redirección entrada, salida o las 2
};
typedef struct CommandLine CommandLine;
enum {
	MaxWord = 4*1024, // 4k  
};

struct CommandLine{

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
    int status; // si es error, si hay redirección entrada, salida o las 2
};
typedef struct CommandLine CommandLine;
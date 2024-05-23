enum {
	MaxWords = 1024,
	MaxWord = 4 * 1024,	// 4k  
};

enum {
	ZeroArgs,
	MaxLine = 4 * 1024,	// 4k  
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
struct CommandLine {

	char **words;		// eje: [cat, /tmp/a, |, wc, -c, >, x, <, y, &]
	long long numwords;	// eje: 10
	char ***commands;	// eje: [[cat, /tmp/a], [wc, -c]]
	long long *numsubcommands;	// eje: [2,2]
	long long numcommands;	// eje: 2 SOLO se actualiza SI hay PIPES
	int bg;
	int inrednum;		// 1
	int inredfd;		// eje: decriptor de fichero creado para y
	char *inred;		// eje: y
	int outrednum;		// 1
	int outredfd;		// eje: descriptor de fichero creado para x
	char *outred;		// eje: x
	int numpipes;
	int **pipesfd;		// hay un pipe con 2 partes[READ, WRITE]
	int status;
	int statusred;
	// status del comando si  es un builtin sin pipes 
	int statusbt;		// inicializada a -1 y SOLO se actualiza si NO HAY PIPES
	// status de cada comando si hay pipes y son builtins
	int *statuspipesbt;	// eje: [-1, -1] al ser ningún comando un builtin
	// SOLO se actualiza SI hay PIPES 
	pid_t *waitpids;	// array que contiene todos los pids de los comandos
	// que no son builtn ins

	// OPCIONALES
	int heredoc;
	int *herepipe;
};
typedef struct CommandLine CommandLine;

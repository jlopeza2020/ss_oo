//#include "parser.h"
// Aquí iré dejando todos los buitlins
//#include "structs.h"
//enum {

//	MaxWord = 4*1024, // 4k  
//};

enum {
	CD,  
};

//int isenv(char *str);
int isequal(char *str);
void executecommands(CommandLine *cl);
//void configvariables(CommandLine *cl);

void setequal(char *str);
//void setenvvar(char *Str);
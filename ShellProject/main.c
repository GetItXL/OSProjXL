#include <stdio.h>
#include <string.h>
#include "Scanner.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
// extern "C" {
// 	int yyparse(void);
// }

int main(void) 
{
	
	//shell_init();
	//shell_init();

	
		shell_init();
		yyparse();			// execute whatever grammer rule in yacc file 
		
	
	
	return 0;
	
}

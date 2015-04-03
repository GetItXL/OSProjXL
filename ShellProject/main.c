#include <stdio.h>
#include <string.h>
#include "Scanner.h"
#include "Scanner.c"

extern int yylex();
extern int yylineno;
extern char* yytext;
// extern "C" {
// 	int yyparse(void);
// }
int CMD = 0;

int main(void) 
{
	
	//shell_init();
//	printPrompt();
//	yyparse();

	while(1){
		printPrompt();
		CMD = getCommand();
		switch(CMD)		//check if its a excutible command that follows all the rules.
		{
			case (OK)	:	processCommand();
							break;
			default		:	recover_from_errors();
		}
		
	}
	
	//return yyparse();
		//int i = yyparse();			// execute whatever grammer rule in yacc file 
		//printf("%d",i);
	
	
	return 0;
	
}



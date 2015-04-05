%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Scanner.h"

int yydebug=1; 


extern int yylex();
void yyerror (char *s);


//#define YYDEBUG_LEXER_TEXT yytext
int yylineno;
int yydebug;



%}

%union {
	int num;
	char *string;
}

/* default yylval type is int (num) */
%token <num> BYE SETENV PRINTENV CD UNSETENV ALIAS UNALIAS EOL
%token <string> WORD METACHAR


	
%%	
	
	cmd 		
			:	builtin_cmd				{	;}
			;

	builtin_cmd		
			:	ALIAS EOL				{ 	;}
			| 	PRINTENV EOL			
				{ 	bicmd = PRINTENV;
					builtin = 1;
					printf("PRINTENV\n");
					YYACCEPT; 
					}
			|	SETENV EOL		
			|	CD EOL					
				{ 	bicmd = CDHOME;
					builtin = 1;
					printf("CD no para\n");
					YYACCEPT; 
					}
			|	CD WORD EOL
				{
					bicmd = CDX;
					builtin = 1;
					bistr = $2;
					printf("CD para\n");
					YYACCEPT;
				}
			|	BYE EOL
				{
					bicmd = BYE;
					builtin = 1;
					printf("BYE\n");
					YYACCEPT;
				}
			;





%%	


void yyerror (char *s) 
{
	fprintf(stderr, "line %d: %s\n", yylineno, s);
} 

/*
int main(void)
{
	//yydebug= 1;
	int temp = yyparse();
	//printf("yyparse is %d \n", temp);

	return temp;
}*/

/*
	words : WORD
			| words WORD
				;*/


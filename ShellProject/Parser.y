%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.c"


extern int yylex();
void yyerror (char *s);

#define YYDEBUG_LEXER_TEXT yytext
int yylineno;

char sym[10];

%}

%union {char *id;}

%token <id> METACHAR WHITESPACE BYE
%token <id> CD HOMECD SETENV UNSETENV ALIAS UNALIAS PRINTENV
%type  <id>  stm expr quit

	
%%	
	
	stm 	:	expr			{	printf(">> token value : %s\n",$1); YYACCEPT;}
			|	quit			{	printf("token value : %s\n",$1); exit(1);}
			;
			;
	
	expr	:	ALIAS 		{ 	$$ = $1; }
			| 	PRINTENV 	{ 	$$ = $1; }
			|	SETENV 		{ 	$$ = $1; }
			|	CD 			{ 	$$ = $1; }
			;

	quit	: BYE 			{	$$ = $1; }
			;




%%	


void yyerror (char *s) 
{
	fprintf(stderr, "line %d: %s\n", yylineno, s);
	exit(1);
} 

/*

int main(void)
{
	//yydebug= 1;
	int temp = yyparse();
	printf("yyparse is %d", temp);
	
}

*/

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.c"


extern int yylex();
void yyerror (char *s);
char* combine(char *s, char *b, char *c);

//#define YYDEBUG_LEXER_TEXT yytext
int yylineno;
int yydebug;

char sym[100];

%}

%union {char *id;}

%token END
%token <id> METACHAR WORD WHITESPACE BYE 
%token <id> CD HOMECD SETENV UNSETENV ALIAS UNALIAS PRINTENV
%type  <id> line expr buildin quit word combination

	
%%	
	
	line 		
			:	expr					{	printf("<token value : %s >\n",$1); YYACCEPT;}
			|	quit					{	 exit(1);	}
			|	word 					{	printf("<token value : %s >\n",$1); YYACCEPT;}
			|	combination				{	printf("<token value : %s >\n",$1); YYACCEPT;}
			|	word 					{	YYABORT;	}
			;

	expr		
			:	ALIAS 		END			{ 	$$ = $1; printf("token end.");}
			| 	PRINTENV 	END			{ 	$$ = $1; }
			|	SETENV 		END			{ 	$$ = $1; }
			|	CD 			END			{ 	$$ = $1; }
			;

	combination		:	buildin  WHITESPACE word 	{ 	$$ = combine($1, $2, $3);		}	
					;

	buildin		:	ALIAS 					{ 	$$ = $1; }
				| 	PRINTENV 				{ 	$$ = $1; }
				|	SETENV 					{ 	$$ = $1; }
				|	CD 						{ 	$$ = $1; }
				;

	word	:	WORD 	END				{	$$ = $1; }
			;

	quit	: BYE 						{	$$ = $1; }
			;

%%	


void yyerror (char *s) 
{
	fprintf(stderr, "line %d: %s\n", yylineno, s);
} 

char* combine(char *s, char *b, char *c)
{
	int length = strlen(s);
	s[length] = ' '; 			// overwrite null termination
	s[length+1] = '\0'; 		// add a new null termination

	strcpy(sym,s);
	strcat(sym, c);
	return sym;
}

/*
int main(void)
{
	//yydebug= 1;
	int temp = yyparse();
	//printf("yyparse is %d \n", temp);

	return temp;
}
*/

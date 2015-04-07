%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Scanner.c"
int yydebug=1; 


extern int yylex();

void yyerror (char *s);
//char* noquote(char *s);

//#define YYDEBUG_LEXER_TEXT yytext
int yylineno;
int yydebug;

//currcmd = 0;


%}

%union {
	int num;
	char *string;
}

/* default yylval type is int (num) */
%token <num> BYE SETENV PRINTENV CD UNSETENV ALIAS UNALIAS EOL READFILE OUTFILE PIPE BACKGROUND DOL LBRAC RBRAC
%token <string> WORD METACHAR STRING



%%	
	
	cmd 		
			:	builtin_cmd				{	;}
			|	alias_cmd				{	;}
			|	other_cmd				{	;}
			;

	builtin_cmd		
			:	EOL
				{
					builtin = 1;
					bicmd = EOL;
					YYACCEPT;
				}
			| 	PRINTENV EOL			
				{ 	bicmd = PRINTENV;
					builtin = 1;
					printf("PRINTENV\n");
					YYACCEPT; 
				}
			|	SETENV WORD WORD EOL	
				{
					bicmd = SETENV;
					builtin = 1;
					bistr = $2;
					bistr2 = $3;
					printf("SETENV\n");
					YYACCEPT;
				}	
			|	UNSETENV WORD EOL
				{
					bicmd = UNSETENV;
					builtin = 1;
					bistr = $2;
					printf("UNSETENV\n");
					YYACCEPT;
				}
			|	CD EOL					
				{ 	
					bicmd = CDHOME;
					builtin = 1;
					printf("CD no para\n");
					YYACCEPT; 
				}
			|	CD 					
				{ 	
					bicmd = CDHOME;
					builtin = 1;
					printf("CD no para\n");
					YYACCEPT; 
				}
			|	CD WORD 
				{
					bicmd = CDX;
					builtin = 1;
					bistr = $2;
					printf("CD para\n");
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
			|	BYE 
				{
					bicmd = BYE;
					builtin = 1;
					printf("BYE\n");
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


	alias_cmd	
			:	ALIAS EOL				
				{ 	
					bicmd = ALIAS;
					builtin = 1;
					printf("ALIAS no para\n");
					YYACCEPT;
				}
			|	ALIAS WORD STRING EOL
				{
					bicmd = ALIASADDSTR;
					builtin = 1;
					aliasname = $2;
					aliastr = $3;
					printf("ALIASADDSTR %s\n",$3);
					YYACCEPT;
				}
			|	ALIAS WORD WORD EOL			// alias to another alias
				{
					bicmd = ALIASADD;
					builtin = 1;
					aliasname = $2;
					aliastr = $3;
					printf("ALIASADDWORD %s\n",$3);
					YYACCEPT;
				}
			|	UNALIAS WORD EOL
				{
					bicmd = UNALIAS;
					builtin = 1;
					aliasname = $2;
					printf("UNALIAS no para\n");
					YYACCEPT;
				}

			;
	

	other_cmd:
				WORD EOL			//the word here may be alias or system call command so we need to call something
				{
					builtin = 0;
					comtab[currcmd].comName = $1;
					comtab[currcmd].countArgs = 0;
					//comtab[currcmd].atptr = Allocate(ARGTAB);

					unknowStr = $1;
					printf("WORD para\n");
					YYACCEPT;
				}
			|	WORD 				//testing code
				{
					builtin = 0;
					comtab[currcmd].comName = $1;
					comtab[currcmd].countArgs = 0;
					//comtab[currcmd].atptr = Allocate(ARGTAB);

					unknowStr = $1;
					printf("WORD without endline\n");
					YYACCEPT;
				}
			|	STRING 				//testing code
				{
					bicmd = STRING;
					builtin = 0;
					unknowStr = $1;
					printf("STRING without endline\n");
					YYACCEPT;
				}
			|	STRING 	EOL			//testing code
				{
					bicmd = STRING;
					builtin = 0;
					unknowStr = $1;
					printf("STRING  endline\n");
					YYACCEPT;
				}
			;







%%	

/*
char* noquoto(char* s)
{
	char temp[100];
	int length = strlen(s);
	printf("length is %d\n", length);

	for(int i = 0; i < length-2; i++)
	{
		temp[i] = s[i+1];
		//printf("char %c\n", temp[i-1]);
	}
	printf("new string %s\n", temp);


	return temp;
}
*/


void yyerror (char *s) 
{
	fprintf(stderr, "line %d: %s\n", yylineno, s);
} 



/*
	words : WORD
				{
					$$ = $1;
				}
			| words WORD
				;*/



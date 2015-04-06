%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Scanner.c"
int yydebug=1; 


extern int yylex();
void yyerror (char *s);

char* noquote(char *s);

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
%token <string> WORD METACHAR STRING



%%	
	
	cmd 		
			:	builtin_cmd				{	;}
			|	alias_cmd				{	;}
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
			|	CD WORD EOL
				{
					bicmd = CDX;
					builtin = 1;
					bistr = $2;
					printf("CD para\n");
					YYACCEPT;
				}
			|	WORD EOL			//the word here may be alias so we need to call something
				{
					bicmd = WORD;
					builtin = 1;
					strAlias = $1;
					printf("WORD para\n");
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
			|	ALIAS WORD WORD EOL				// alias to another alias
				{
					bicmd = ALIASADD;
					builtin = 1;
					aliasname = $2;
					aliastr = $3;
					printf("ALIASADDWORD %s\n",$3);
					YYACCEPT;
				}
			|	ALIAS WORD BYE EOL				// alias to another builin command (bye/cd/whatever)
				{
					bicmd = ALIASADD;
					builtin = 1;
					aliasname = $2;
					aliastr = "bye";
					YYACCEPT;
				}
			|	ALIAS WORD CD EOL				// alias to another builin command (bye/cd/whatever)
				{
					bicmd = ALIASADD;
					builtin = 1;
					aliasname = $2;
					aliastr = "cd";
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
int main(void)
{
	//yydebug= 1;
	int temp = yyparse();
	//printf("yyparse is %d \n", temp);

	return temp;
}*/


/*
	words : WORD
				{
					$$ = $1;
				}
			| words WORD
				;*/



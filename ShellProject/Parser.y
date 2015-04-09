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
%type <string> cmd_name other_cmd pipe_cmd



%%	
	
	cmd 		
			:	builtin_cmd				{	;}
			|	alias_cmd				{	;}
			|	pipe_cmd				{	;}
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
			| 	PRINTENV 			
				{ 	bicmd = PRINTENV;
					builtin = 1;
					printf("PRINTENV no eol\n");
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
			|	SETENV WORD WORD 	
				{
					bicmd = SETENV;
					builtin = 1;
					bistr = $2;
					bistr2 = $3;
					printf("SETENV no eol\n");
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
			|	UNSETENV WORD 
				{
					bicmd = UNSETENV;
					builtin = 1;
					bistr = $2;
					printf("UNSETENV no eol\n");
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
					printf("CD no para no eol\n");
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
			|	ALIAS 				
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
			|	ALIAS WORD STRING 
				{
					bicmd = ALIASADDSTR;
					builtin = 1;
					aliasname = $2;
					aliastr = $3;
					printf("ALIASADDSTR %s\n no eol",$3);
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
			|	ALIAS WORD WORD 			// alias to another alias
				{
					bicmd = ALIASADD;
					builtin = 1;
					aliasname = $2;
					aliastr = $3;
					printf("ALIASADDWORD %s\n no eol",$3);
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
			|	UNALIAS WORD 
				{
					bicmd = UNALIAS;
					builtin = 1;
					aliasname = $2;
					printf("UNALIAS no para no eol\n");
					YYACCEPT;
				}

			;
	
	pipe_cmd:	
				other_cmd EOL
				{
					builtin = 0;
					//maybe check here if there is a builin command?
					unknowStr = $1;
					printf(" pipline with 1 command\n");
					YYACCEPT;
				}
			|	other_cmd 
				{
					builtin = 0;
					unknowStr = $1;
					printf(" pipline with 1 command no eol\n");
					YYACCEPT;
				}
			|	other_cmd PIPE other_cmd
				{
					builtin = 0;
					unknowStr = $1;
					//numbCmd++;
					printf(" pipline with 2 command no eol\n");
					YYACCEPT;
				}
			// this won't work, need help with the repeatable pipe
			|	pipe_cmd PIPE other_cmd
				{
					builtin = 0;
					unknowStr = $1;
					printf(" pipline with more commands no eol\n");
					YYACCEPT;
				}
			;

	other_cmd
			:	
				cmd_name					//For cmd without any arg
				{
					builtin = 0;
					comtab[numbCmd].comName = $1;
					comtab[numbCmd].countArgs = 0;
					//comtab[numbCmd].atptr = Allocate(ARGTAB);
					numbCmd++;
					unknowStr = $1;
					printf("nonbuiltin without arg\n");
					//YYACCEPT;

				}
			|	cmd_name arguments			//with arguments
				{
					builtin = 0;
					comtab[numbCmd].comName = $1;
					//comtab[numbCmd].countArgs = 0;
					//comtab[numbCmd].atptr = Allocate(ARGTAB);
					numbCmd++;
					unknowStr = $1;
					printf("nonbuiltin with arg\n");
					//YYACCEPT;
				}	
			;



	//Added the string case in only for the unknownStr, maybe don't need it??
	cmd_name : 	WORD 	
				{
					$$ = $1; 
					printf("name  lala, %d\n",numbCmd);

				}
			 ;


	//Arguments can be one or more WORD/STRING
	arguments
			:	READFILE
				{
					currarg = 1; //first element in arg[] is the cmd 
					comtab[numbCmd].args[currarg] = $1;
					currarg++;
					comtab[numbCmd].countArgs++;
				}
			|	OUTFILE
				{
					currarg = 1; //first element in arg[] is the cmd 
					comtab[numbCmd].args[currarg] = $1;
					currarg++;
					comtab[numbCmd].countArgs++;
				}
			|	WORD 				
				{
					currarg = 1; //first element in arg[] is the cmd 
					comtab[numbCmd].args[currarg] = $1;
					currarg++;
					comtab[numbCmd].countArgs++;
					printf("arge  lala\n");

				}
			|	STRING 				
				{
					currarg = 1; //first element in arg[] is the cmd 
					comtab[numbCmd].args[currarg] = $1;
					currarg++;
					comtab[numbCmd].countArgs++;
				}
			|	arguments WORD
				{
					comtab[numbCmd].args[currarg] = $2;
					currarg++;
					comtab[numbCmd].countArgs++;
				}

			|	arguments STRING
				{
					comtab[numbCmd].args[currarg] = $2;
					currarg++;
					comtab[numbCmd].countArgs++;
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

/*
	cmd_name
			:	WORD
			|	STRING
			|	ENV
			;
	//May need this structure for cmd like this: ${ENV}
*/


/* don't think we still need this
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

							|	STRING 	EOL			//testing code
				{
					bicmd = STRING;
					builtin = 0;
					unknowStr = $1;
					printf("STRING  endline\n");
					YYACCEPT;
				}
*/


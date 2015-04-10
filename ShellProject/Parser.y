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
%token <num> BYE SETENV PRINTENV CD UNSETENV ALIAS UNALIAS GT LT PIPE AMP DOL LBRAC RBRAC
%token <string> WORD METACHAR STRING EOL GTGT TWO AMPONE TILDE
%type <string> cmd_name other_cmd pipe_cmd arguments builtin_cmd end_char



%%	

	/*** Problem with TWO **/
	//TWO is recognized before WORD. Thus if a WORD is 2, then it will be regonized as TWO
	
	cmd 		
			:	
				pipe_cmd EOL				{	//printf("%s\n", $1); 
											YYACCEPT;}
			|	pipe_cmd AMP 				{	printf("&\n"); YYACCEPT;}
			|	pipe_cmd end_line_option	{	YYACCEPT;}


			/*
			pipe_cmd GT WORD EOL
				{
					//Handle output redirection
					comtab[numbCmd-1].outfd = 1;
					comtab[numbCmd-1].outfile = $3;


					printf("cmd > %s\n", $3);
					YYACCEPT;
					
				}
			|	pipe_cmd LT WORD EOL
				{
					//Handle io redirect in
					printf("cmd < %s\n", $3);
					YYACCEPT;
					
				}
			|	pipe_cmd LT WORD GT WORD EOL
				{
					//Handle both
					printf("cmd < %s > %s\n", $3, $5);
					YYACCEPT;

				}
			|	pipe_cmd LT WORD GT WORD BACKGROUND
				{
					//handle both
					//handle background
					printf("cmd < %s > %s &\n", $3, $5);
					YYACCEPT;
				}*/



			//Can & appear in the middle of the pipeline? NO



			|	builtin_cmd				{	;}								
			|	alias_cmd				{	;}

			;


	end_line_option
			:	LT WORD end_char
				{
					//Handle io redirect in
					//printf("cmd < %s\n", $3);
					//YYACCEPT;
					printf("< infile %s\n", $3);


					/**!!!**/
					//put biinfile = ..;
					//and comtab[currcmd].infile = ...;
					//It's ok to put both of them here because system checks if a cmd is built in or not FIRST
				}
			|	LT WORD GTGT WORD end_char
				{
					//
					printf("< infile >> errout %s\n", $5);
				}
			|	LT WORD GT WORD end_char
				{
					//Handle both
					//printf("cmd < %s > %s\n", $3, $5);
					//YYACCEPT;
					printf("< infile > outfile %s\n", $5);
				}
			|	LT WORD GT WORD TWO GT WORD end_char
				{
					printf("< infile > outfile 2>file %s\n", $8);
				}
			|	LT WORD GT WORD TWO GT AMPONE end_char
				{
					printf("< infile > outfile 2>&1 %s\n", $8);
				}
			|	GT WORD end_char
				{
					//Handle output redirection
					comtab[numbCmd-1].outfd = 1;
					comtab[numbCmd-1].outfile = $2;

					//For builtin
					bioutf = 1; //Indicates that there is a outfile redirection
					biOutfile = $2;

					//printf("cmd > %s\n", $3);
					//YYACCEPT;		
					printf("> outfile %s\n", $3);			
				}
			|	GT WORD TWO GT WORD end_char
				{
					printf("> outfile 2>file %s\n", $6);
				}
			|	GT WORD TWO GT AMPONE end_char
				{
					printf("> outfile 2>&1 %s\n", $6);
				}
			|	GTGT WORD end_char
				{
					printf(">> errout %s\n", $3);
				} 
				//!!!!!Can you have & after >>?
			|	TWO GT WORD end_char
				{
					printf("2>file %s\n", $4);
				}
			|	TWO GT AMPONE end_char
				{
					printf("2>&1 %s\n", $4);
				}
			;



	end_char
			: 	EOL
				{
					//
					//printf("EOL ");
					$$ = "EOL";
				}
			|	AMP
				{
					//background flag
					//printf("& ");
					$$ = "AMP";
				}
			;


	builtin_cmd		
			:	EOL
				{
					builtin = 1;
					bicmd = EOL;
					YYACCEPT;

				}
			|	PRINTENV EOL		
				{ 	
					bicmd = PRINTENV;
					builtin = 1;
					printf("PRINTENV no eol\n");
					YYACCEPT; 
				}

			|	PRINTENV end_line_option
				{
					bicmd = PRINTENV;
					builtin = 1;
					YYACCEPT;
				}

			|	SETENV WORD WORD EOL	
				{
					bicmd = SETENV;
					builtin = 1;
					bistr = $2;
					bistr2 = $3;
					printf("SETENV no eol\n");
					YYACCEPT;
				}	
		
			|	SETENV WORD WORD end_line_option
				{
					bicmd = SETENV;
					builtin = 1;
					bistr = $2;
					bistr2 = $3;
					YYACCEPT;
				}

			|	UNSETENV WORD EOL
				{
					bicmd = UNSETENV;
					builtin = 1;
					bistr = $2;
					printf("UNSETENV no eol\n");
					YYACCEPT;
				}
			
			|	UNSETENV WORD end_line_option
				{
					bicmd = UNSETENV;
					builtin = 1;
					bistr = $2;
					YYACCEPT;
				}

			|	CD EOL			
				{ 	
					bicmd = CDHOME;
					builtin = 1;
					printf("CD no para no eol\n");
					YYACCEPT; 
			
				}
			
			|	CD end_line_option
				{
					bicmd = CDHOME;
					builtin = 1;

					YYACCEPT; 
				}

			|	CD TILDE EOL			
				{ 	
					bicmd = CDHOME;
					builtin = 1;
					printf("CD tilde\n");
					YYACCEPT; 
			
				}

			|	CD TILDE end_line_option
				{
					bicmd = CDHOME;
					builtin = 1;

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

			|	CD WORD end_line_option
				{
					bicmd = CDX;
					builtin = 1;
					bistr = $2;

					YYACCEPT;
				}	

			|	BYE 
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


	
	pipe_cmd:	//Checking if a cmd is a valid cmd needs to be done in Scanner.c
				
				other_cmd
				{
					builtin = 0;
					//maybe check here if there is a builin command?
					unknowStr = $1;
					//printf(" pipline with 1 command\n");
					//YYACCEPT;
					//Can't do yy accept here?
					
					/*** Testing ***/
					//$$ = $1;
				}



			|	pipe_cmd PIPE other_cmd
				{
					builtin = 0;
					unknowStr = $1;
					//numbCmd++;
					//printf(" pipline with 2 command no eol\n");
					//YYACCEPT;

					/*** Testing ***/
					//char *temp = strcat($1, " | ");
					//$$ = strcat(temp, $3);
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
					//printf("nonbuiltin without arg\n");
					//YYACCEPT;



					/*** Testing ***/
					//$$ = $1;

				}
			|	cmd_name arguments			//with arguments
				{
					builtin = 0;
					comtab[numbCmd].comName = $1;
					//comtab[numbCmd].countArgs = 0;
					numbCmd++;
					unknowStr = $1;
					//printf("nonbuiltin with arg\n");
					//YYACCEPT;


					/*** Testing ***/
					//char *temp = strcat($1, " ");
					//$$ = strcat(temp, $2);
				}	
			;



	//Added the string case in only for the unknownStr, maybe don't need it??
	cmd_name : 	WORD 	
				{
					$$ = $1; //This is not for testing. IT WAS HERE!
					//printf("name  lala, %d\n",numbCmd);


				}
			 ;


	//Arguments can be one or more WORD/STRING
	arguments
			:	WORD 				
				{
					currarg = 1; //first element in arg[] is the cmd 
					comtab[numbCmd].args[currarg] = $1;
					currarg++;
					comtab[numbCmd].countArgs++;
					//printf("arge  lala\n");



					/*** Testing ***/
					//$$ = $1;


				}
			|	STRING 				
				{
					currarg = 1; //first element in arg[] is the cmd 
					comtab[numbCmd].args[currarg] = $1;
					currarg++;
					comtab[numbCmd].countArgs++;

					/*** Testing ***/
					//$$ = $1;

				}
			|	arguments WORD
				{
					comtab[numbCmd].args[currarg] = $2;
					currarg++;
					comtab[numbCmd].countArgs++;

					/*** Testing ***/
					//char *temp = strcat($1, " ");
					//$$ = strcat(temp, $2);
				}

			|	arguments STRING
				{
					comtab[numbCmd].args[currarg] = $2;
					currarg++;
					comtab[numbCmd].countArgs++;


					/*** Testing ***/
					//char *temp = strcat($1, " ");
					//$$ = strcat(temp, $2);
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


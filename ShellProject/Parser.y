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


int isWildcardPattern(char*);

%}

%union {
	int num;
	char *string;
}

/* default yylval type is int (num) */
%token <num> BYE SETENV PRINTENV CD UNSETENV ALIAS UNALIAS GT LT PIPE AMP DOL LBRAC RBRAC
%token <string> WORD METACHAR STRING EOL GTGT TWO AMPONE TILDE
%type <string> cmd_name other_cmd pipe_cmd arguments builtin_cmd end_char end_line_option env_var



%%	

	/*** Problem with TWO **/
	//TWO is recognized before WORD. Thus if a WORD is 2, then it will be regonized as TWO
	
	cmd 		
			:

				pipe_cmd end_line_option	{	YYACCEPT;}


			//Can & appear in the middle of the pipeline? NO

			|	builtin_cmd				{	;}								
			|	alias_cmd				{	;}

			;


	end_line_option
			:	LT WORD end_char
				{
					//Builtin
					biinf = 1;
					biInfile = $2;

					//nonbuiltin
					comtab[0].infd = 1;		//input redirect is for the first cmd in the comtab
					comtab[0].infile = $2;
					


					/**!!!**/
					//put biinfile = ..;
					//and comtab[currcmd].infile = ...;
					//It's ok to put both of them here because system checks if a cmd is built in or not FIRST
					printf("< infile %s\n", $3);
				}
			|	LT WORD GTGT WORD end_char
				{
					//builtin
					biinf = 1;
					biInfile = $2;
					bioutf = 2; //append
					biOutfile = $4;

					//nonbuiltin
					comtab[0].infd = 1;		//input redirect is for the first cmd in the comtab
					comtab[0].infile = $2;
					comtab[numbCmd-1].outfd = 2; //append
					comtab[numbCmd-1].outfile = $4;


					printf("< infile >> errout %s\n", $5);
				}
			|	LT WORD GT WORD end_char
				{
					//builtin
					biinf = 1;
					biInfile = $2;
					bioutf = 1; //append
					biOutfile = $4;

					//nonbuiltin
					comtab[0].infd = 1;		//input redirect is for the first cmd in the comtab
					comtab[0].infile = $2;
					comtab[numbCmd-1].outfd = 1; //append
					comtab[numbCmd-1].outfile = $4;


					printf("< infile > outfile %s\n", $5);
				}
			|	LT WORD GT WORD TWO GT WORD end_char
				{
					//builtin
					biinf = 1;
					biInfile = $2;
					bioutf = 1; //append
					biOutfile = $4;

					//nonbuiltin
					comtab[0].infd = 1;		//input redirect is for the first cmd in the comtab
					comtab[0].infile = $2;
					comtab[numbCmd-1].outfd = 1; //append
					comtab[numbCmd-1].outfile = $4;

					//Need to handle 2>file

					printf("< infile > outfile 2>file %s\n", $8);
				}
			|	LT WORD GT WORD TWO GT AMPONE end_char
				{
					//builtin
					biinf = 1;
					biInfile = $2;
					bioutf = 1; //append
					biOutfile = $4;

					//nonbuiltin
					comtab[0].infd = 1;		//input redirect is for the first cmd in the comtab
					comtab[0].infile = $2;
					comtab[numbCmd-1].outfd = 1; //append
					comtab[numbCmd-1].outfile = $4;

					//Need to handle 2>&1

					printf("< infile > outfile 2>&1 %s\n", $8);
				}
			|	GT WORD end_char
				{
					//nonbuiltin
					comtab[numbCmd-1].outfd = 1;
					comtab[numbCmd-1].outfile = $2;

					//builtin
					bioutf = 1; //Indicates that there is a outfile redirection
					biOutfile = $2;
	
					printf("> outfile %s\n", $3);			
				}
			|	GT WORD TWO GT WORD end_char
				{
					//nonbuiltin
					comtab[numbCmd-1].outfd = 1;
					comtab[numbCmd-1].outfile = $2;

					//builtin
					bioutf = 1; //Indicates that there is a outfile redirection
					biOutfile = $2;

					//Need to handle 2>file

					printf("> outfile 2>file %s\n", $6);
				}
			|	GT WORD TWO GT AMPONE end_char
				{
					//nonbuiltin
					comtab[numbCmd-1].outfd = 1;
					comtab[numbCmd-1].outfile = $2;

					//builtin
					bioutf = 1; //Indicates that there is a outfile redirection
					biOutfile = $2;

					//Need to handle 2>&1

					printf("> outfile 2>&1 %s\n", $6);
				}
			|	GTGT WORD end_char
				{
					//nonbuiltin
					comtab[numbCmd-1].outfd = 2;
					comtab[numbCmd-1].outfile = $2;

					//builtin
					bioutf = 2; //Indicates that there is a outfile redirection
					biOutfile = $2;
					printf(">> errout %s\n", $3);
				} 

			|	TWO GT WORD end_char
				{
					printf("2>file %s\n", $4);
				}
			|	TWO GT AMPONE end_char
				{
					printf("2>&1 %s\n", $4);
				}
			|	end_char	{$$ = $1;}
			;



	end_char
			: 	EOL
				{
					//
					//printf("EOL ");
					$$ = "EOL";
				}
			|	AMP EOL
				{
					//background flag
					printf("& ");
					$$ = "AMP";
					amp = 1;
				}
			;

	builtin_cmd		
			:	EOL
				{
					builtin = 1;
					bicmd = EOL;
					YYACCEPT;

				}

			|	PRINTENV end_line_option
				{
					bicmd = PRINTENV;
					builtin = 1;
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

			|	SETENV WORD STRING end_line_option
				{
					bicmd = SETENV;
					builtin = 1;
					bistr = $2;
					bistr2 = $3;
					printf("SETENV string TEST: %s", $3);
					YYACCEPT;
				}
			
			|	UNSETENV WORD end_line_option
				{
					bicmd = UNSETENV;
					builtin = 1;
					bistr = $2;
					YYACCEPT;
				}
			
			|	CD end_line_option
				{
					bicmd = CDHOME;
					builtin = 1;
					bistr = $2;
					YYACCEPT; 
				}


			|	CD TILDE end_line_option
				{
					bicmd = CDHOME;
					builtin = 1;

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
					//unknowStr = $1;
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
			|	env_var
				{
					$$ = $1;
					printf("cmd is envar: %s\n", $1);
				}
			 ;


	//Arguments can be one or more WORD/STRING
	arguments
			:	WORD 				
				{
					
					currarg = 1; //first element in arg[] is the cmd 

					if(isWildcardPattern($1)){
						glob_t result;
						
						//NOSORT = sorted alphabetical order?
						if(glob($1, GLOB_NOSORT | GLOB_NOCHECK, NULL, &result) == 0){
							int i; //gl.pathc contains num of matched files
							for(i = 0; i < result.gl_pathc; i++){
								comtab[numbCmd].args[currarg] = strdup(result.gl_pathv[i]);
								comtab[numbCmd].countArgs++;
								currarg++;
							}
							if( result.gl_pathc > 0 )
   								globfree( &result );

							}
					}
					else{
						comtab[numbCmd].args[currarg] = $1;
						currarg++;
						comtab[numbCmd].countArgs++;
					}


					
					
					//printf("arge  lala\n");



					/*** Testing ***/
					//$$ = $1;


				}
			|	STRING 				
				{


					currarg = 1; //first element in arg[] is the cmd 

					if(isWildcardPattern($1)){
						glob_t result;
						
						//NOSORT = sorted alphabetical order?
						if(glob($1, GLOB_NOSORT | GLOB_NOCHECK, NULL, &result) == 0){
							int i; //gl.pathc contains num of matched files
							for(i = 0; i < result.gl_pathc; i++){
								comtab[numbCmd].args[currarg] = strdup(result.gl_pathv[i]);
								comtab[numbCmd].countArgs++;
								currarg++;
							}
							if( result.gl_pathc > 0 )
   								globfree( &result );

							}
					}
					else{
						comtab[numbCmd].args[currarg] = $1;
						currarg++;
						comtab[numbCmd].countArgs++;
					}




					/*	//String doesn't need to check * and ? ???!!!!!
					currarg = 1; //first element in arg[] is the cmd 
					comtab[numbCmd].args[currarg] = $1;
					currarg++;
					comtab[numbCmd].countArgs++;
					*/

					/*** Testing ***/
					//$$ = $1;

				}

			|	env_var
				{
					//env_var does NOT need * and ? ??

					//Do something
					currarg = 1; //first element in arg[] is the cmd 
					comtab[numbCmd].args[currarg] = $1;
					currarg++;
					comtab[numbCmd].countArgs++;
					//printf("arge  lala\n");



					/*** Testing ***/
					//$$ = $1;
					printf("nonbuiltin w 1 envar arg. envar: %s\n", $1);

				}

			|	arguments env_var
				{
					//ENV Var doesn't need * and ? ???
					comtab[numbCmd].args[currarg] = $2;
					currarg++;
					comtab[numbCmd].countArgs++;

					/*** Testing ***/
					//char *temp = strcat($1, " ");
					//$$ = strcat(temp, $2);

					printf("nonbuiltin w multiple args. envar:%s \n ", $2);
				}

			|	arguments WORD
				{
					
					if(isWildcardPattern($2)){
						glob_t result;
						
						//NOSORT = sorted alphabetical order?
						if(glob($2, GLOB_NOSORT | GLOB_NOCHECK, NULL, &result) == 0){
							int i; //gl.pathc contains num of matched files
							for(i = 0; i < result.gl_pathc; i++){
								comtab[numbCmd].args[currarg] = strdup(result.gl_pathv[i]);
								comtab[numbCmd].countArgs++;
								currarg++;
							}
							if( result.gl_pathc > 0 )
   								globfree( &result );
							}
					}
					else{
						comtab[numbCmd].args[currarg] = $2;
						currarg++;
						comtab[numbCmd].countArgs++;
					}

					/*
					comtab[numbCmd].args[currarg] = $2;
					currarg++;
					comtab[numbCmd].countArgs++;*/

					/*** Testing ***/
					//char *temp = strcat($1, " ");
					//$$ = strcat(temp, $2);
				}

			|	arguments STRING
				{
					if(isWildcardPattern($2)){
						glob_t result;
						
						//NOSORT = sorted alphabetical order?
						if(glob($2, GLOB_NOSORT | GLOB_NOCHECK, NULL, &result) == 0){
							int i; //gl.pathc contains num of matched files
							for(i = 0; i < result.gl_pathc; i++){
								comtab[numbCmd].args[currarg] = strdup(result.gl_pathv[i]);
								comtab[numbCmd].countArgs++;
								currarg++;
							}
							if( result.gl_pathc > 0 )
   								globfree( &result );
							}
					}
					else{
						comtab[numbCmd].args[currarg] = $2;
						currarg++;
						comtab[numbCmd].countArgs++;
					}

					/*
					comtab[numbCmd].args[currarg] = $2;
					currarg++;
					comtab[numbCmd].countArgs++;*/


					/*** Testing ***/
					//char *temp = strcat($1, " ");
					//$$ = strcat(temp, $2);
				}
			;

	env_var
			:	DOL LBRAC WORD RBRAC
				{
					$$ = $3;
					
				}
			;






%%	

//Check if an arguments contains * or ?
int isWildcardPattern(char *arg){
	int i;
	for(i = 0; i < strlen(arg); i++){
		if(arg[i] == '*' || arg[i] == '?')
			return 1;
	}

	return 0; //no * or ? found
}



void yyerror (char *s) 
{
	fprintf(stderr, "line %d: %s\n", yylineno, s);
} 


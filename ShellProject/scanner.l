%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "y.tab.h"


// for debug 
#undef ECHO
#define ECHO printf ("[%s]\n", yytext)

%}



%%
					// create an union in yacc file, and char *id/
					// I need to pass the string value from lex token to yacc, so I use yylval.id = strdup(yytext);
					// when we need to use, just use $no. in the yacc rule section.
"bye"							{	yylval.id = strdup(yytext); ECHO; 	return BYE;}
"setenv"						{	yylval.id = strdup(yytext); ECHO;	return SETENV;}
"printenv"						{ 	yylval.id = strdup(yytext); ECHO;	return PRINTENV;}
"cd"							{ 	yylval.id = strdup(yytext); ECHO;	return CD;}
"cd(~)?"						{ 	yylval.id = strdup(yytext);	ECHO;	return HOMECD;}
"unsetenv"						{ 	yylval.id = strdup(yytext);	ECHO;	return UNSETENV;}
"alias"							{ 	yylval.id = strdup(yytext);	ECHO;	return ALIAS;}
"unalias"						{	yylval.id = strdup(yytext);	ECHO;	return UNALIAS;}



[ \t]+							{ECHO;return WHITESPACE;}
\n 								;
.								{ECHO;printf("unexpected character\n");}

%%


int yywrap() {return -1;}
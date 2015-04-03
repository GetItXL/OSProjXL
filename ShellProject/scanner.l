%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "y.tab.h"


// for debug 
#undef ECHO		// this will return the token value once pass to yacc file
//#define ECHO printf ("[%s]\n", yytext)

%}



%%
					// create an union in yacc file, and char *id/
					// I need to pass the string value from lex token to yacc, so I use yylval.id = strdup(yytext);
					// when we need to use, just use $no. in the yacc rule section.
"bye"							{	yylval.id = strdup(yytext);  	return BYE;}
"setenv"						{	yylval.id = strdup(yytext); 	return SETENV;}
"printenv"						{ 	yylval.id = strdup(yytext); 	return PRINTENV;}
"cd"							{ 	yylval.id = strdup(yytext); 	return CD;}
"cd(~)?"						{ 	yylval.id = strdup(yytext);		return HOMECD;}
"unsetenv"						{ 	yylval.id = strdup(yytext);		return UNSETENV;}
"alias"							{ 	yylval.id = strdup(yytext);		return ALIAS;}
"unalias"						{	yylval.id = strdup(yytext);		return UNALIAS;}



[a-zA-Z0-9]+					{	yylval.id = strdup(yytext);		return WORD;	}
["][a-zA-Z0-9<>|\\& ]+["]  		{ 	yylval.id = strdup(yytext);	 	return WORD;	}
[<>|"\\&]						{	yylval.id = strdup(yytext);	 	return METACHAR;	}
[ \t]+							{	ECHO;return WHITESPACE;		}
\n 								{	ECHO;return END;		}
.								{	ECHO;printf("unexpected character\n");}

%%


int yywrap() {return -1;}
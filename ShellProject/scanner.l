%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "y.tab.h"

// for debug 
#undef ECHO		// this will return the token value once pass to yacc file
//#define ECHO printf ("[%s]\n", yytext)


//String needs ? and * ???
%}


%%
					// create an union in yacc file, and char *id/
					// I need to pass the string value from lex token to yacc, so I use yylval.id = strdup(yytext);
					// when we need to use, just use $no. in the yacc rule section.

"bye"							{	return BYE;}
"setenv"						{	return SETENV;}
"printenv"						{ 	return PRINTENV;}
"cd"							{ 	return CD;}
"unsetenv"						{ 	return UNSETENV;}
"alias"							{ 	return ALIAS;}
"unalias"						{	return UNALIAS;}
"<"								{	return LT;}
">"								{	return GT;}
"|"								{	return PIPE;}
"&"								{	return AMP;}
"$"								{	return DOL;}
"{"								{	return LBRAC;}
"}"								{	return RBRAC;}
">>"							{	return GTGT;}
"2"								{	return TWO;}
"2>&1"							{	return TWOAMPONE;}
"~"								{	return TILDE;}

[-a-zA-Z0-9./:~*?_]+				{	yylval.string = strdup(yytext);		return WORD;	}
["][-a-zA-Z0-9<>|\\& ./~:${}*?_]+["]  	{ 	yylval.string = strdup(yytext);	 	return STRING;	}
[<>|"\\&]						{	yylval.string = strdup(yytext);	 	return METACHAR;	}
[ \t]+							;	//Ignore whitespaces
\n								{	return EOL;}
.								{	ECHO;printf("unexpected character\n");}

%%


int yywrap() {return 1;}
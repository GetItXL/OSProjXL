%{
#include <stdio.h>
#include "y.tab.h"
extern YYSTYPE yylval;
%}
%option nounput yylineno

%%

"bye"							return BYE;
[a-zA-Z0-9]+ 					return WORD;
["][a-zA-Z0-9<>|\\& ]+["]  		return WORD;
[ \t]+							return WHITESPACE;
[<>|"\\&]						return METACHAR;

.								printf("unexpected character\n");

%%

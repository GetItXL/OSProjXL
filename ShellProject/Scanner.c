#include <stdio.h>
#include <string.h>
#include "Scanner.h"

extern int yylex();
extern int yylineno;
extern char* yytext;



void print(void)
{
	printf("word\n");
}

void shell_init()
{
	printf("\nCMD < ");
}
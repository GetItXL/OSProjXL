%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Scanner.c"
#include "Scanner.h"
#include "main.c"

void yyerror(const char *str)
{
	fprintf(stderr,"error: %s\n",str);
}

int yywrap()
{
	return 1;
}

%}

%token WORD METACHAR WHITESPACE BYE
	
%%

	
	commands: 
	/* : a command is either empty, or it consists of more commands, followed by a command. They way YACC works means that it can now easily chop off individual command groups (from the front) and reduce them. */
			| commands command    	/* this is left recursive */
			;
	
	
	command: 
			bye|word|metachar|whitespace;

	bye:
				BYE            				{	exit(1);	};
			
	word:
				WORD 						{	print();	};

	metachar:
				METACHAR    				{	printf("metachar\n");	};

	whitespace:
				WHITESPACE 					{	printf("whitespace\n");};	


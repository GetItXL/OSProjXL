%{
#include <stdio.h>
#include <string.h>

void yyerror(const char *str)
{
	fprintf(stderr,"error: %s\n",str);
}

int yywrap()
{
	return 1;
}

main()
{
	yyparse();
}

%}

%token WORD METACHAR WHITESPACE 
	
%%
	
	commands: /* empty */
		| commands command
		;
	
	
	command: 
		word
		|
		metachar
		|
		whitespace
		;
		
	word:
		WORD
		{
			printf("word\t");
		}
		;
	metachar:
		METACHAR
		{
			printf("metachar\t");
		};
	whitespace:
		WHITESPACE
		{
			printf("whitespace\t");
		};	
		
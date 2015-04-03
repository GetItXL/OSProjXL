#include <stdio.h>
#include <string.h>
#include "Scanner.h"

extern int yylex();
extern int yylineno;
extern char* yytext;



void print(void){	printf("word\n");}

void shell_init()
{	
	// init all variables.
	// define (allocate storage) for some var/tables 
	//init all tables (e.g., alias table)
	//get PATH environment variable (use getenv()) 
	//get HOME env variable (also use getenv()) 
	//disable anything that can kill your shell.
	// (the shell should never die; only can be exit) 
	// do anything you feel should be done as init
}

void printPrompt(){	printf("\n>>> ");}
// {
// 	printf("shell < ");
// }

int getCommand()
{
	//init_scanner_and_parser(); 
	//The value is 1 if parsing failed because of invalid input, i.e., input that contains a syntax error or that causes YYABORT to be invoked.
	int temp = yyparse();
	//printf("yyparse is %d \n",temp);

	if (temp)								
		return (ERROR);
	else
		return (OK);		//The value returned by yyparse is 0 if parsing was successfu					
}




void recover_from_errors()
{ 	
	//Find out if error occurs in middle of command 
	//That is the command still has a “tail”
	//In this case you have to recover by “eating” 
	// the rest of the command.
	// To do this: use yylex() directly.
	printf("recover_from_errors");
}

void processCommand()
{	
	// if (builtin) 
	// 	do_it();
	// else
	// 	execute_it();	
	
	printf("processCommand");
}

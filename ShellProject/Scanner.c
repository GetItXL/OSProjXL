#include <stdio.h>
#include <string.h>
#include "Scanner.h"
#include <unistd.h>
#include "alias.c"
//#include <stdlib.h>
//#include "y.tab.h"
#include <signal.h>

//---------------- This is for handling Ctrl+C -------
typedef void (*sighandler_t)(int);
void signalHandling(int signo)
{

	printf("\n");
	printPrompt();
	fflush(stdout);
}

//declare the global variables 
int builtin;
int bicmd;
char *bistr;
char *bistr2;

int CMD;

int main(void) 
{
	//For handling Ctrl + C
	signal(SIGINT, SIG_IGN);
	signal(SIGINT, signalHandling);
	//May need to use sigaction instead later.

	shell_init();
	while(1){
		printPrompt();

		//This calls yyparse()
		CMD = getCommand();
		switch(CMD)		//check if its a excutible command that follows all the rules.
		{
			//case BYE:		exit(1);
			case ERROR: 	recover_from_errors();
							break;
			case OK:		processCommand();
							break;
		}
		
	}
	return 0;
	
}




void print(void){	printf("word\n");}

void shell_init()
{	
	aliasNumb = 0;

	//aliastab[MAX] = {0};
	// init all variables.
	// define (allocate storage) for some var/tables 
	//init all tables (e.g., alias table)
	//get PATH environment variable (use getenv()) 
	//get HOME env variable (also use getenv()) 
	//disable anything that can kill your shell.
	// (the shell should never die; only can be exit) 
	// do anything you feel should be done as init
}

void printPrompt(){	 printf("%s :> ",getcwd(NULL, 0));}
// {
// 	printf("shell < ");
// }

int getCommand()
{
	init_scanner_and_parser(); 

	//The value is 1 if parsing failed because of invalid input, i.e., input that contains a syntax error or that causes YYABORT to be invoked.
	
	// maybe need one variable to check alias here?
	if (yyparse())							
		return (ERROR);
	else
		return (OK);		//The value returned by yyparse is 0 if parsing was successfu					
}

void init_scanner_and_parser(){
	bicmd = 0;
	builtin = 0;
	//set to null
	bistr = '\0';
	bistr2 = '\0';
	aliasname = '\0';
	aliastr = '\0';
	aliasLoop = 0;				//return 1 is loop, 0 is not loop
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
	
	//printf("processCommand\n");
	printf("bicmd: %d\n", bicmd);
	//printf("builtin: %d\n", builtin);

	if(builtin)
		do_it();	
}



void do_it(){
	switch(bicmd){
		case CDX :
			changedir(bistr);
			break;
		case CDHOME :
			gohome();
			break;
		case PRINTENV :
			printEnv();
			break;
		case BYE :
			exit(1);
			break;
		case SETENV :
			setenv(bistr, bistr2, 1);
			break;
		case ALIAS :
			showAlias();
			break;
		case WORD :
			printf("word in scanner.c\n");
			processAlias(strAlias);
			break;
		case UNALIAS :
			deleteAlias(aliasname);
			break;
		case ALIASADDSTR :
			addAlias(aliasname, aliastr);
			break;
		case ALIASADD :
			addAlias(aliasname, aliastr);
			break;
		case EOL:
			break;
		default:	
				printf("unexpected error!");
				//exit(1);

	}
}


// ----------------- functions for cd and env --------------

void changedir(char *s){
	if(chdir(s) == -1){
		printf("%s: ", s);
		printf("not a directory\n");
		return;
	}
}


void gohome(){
	chdir(getenv("HOME"));
}

void printEnv(){
	char **env;
	for (env = environ; *env != 0; env++)
        printf("%s\n", *env);
}


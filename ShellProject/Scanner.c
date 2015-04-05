#include <stdio.h>
#include <string.h>
#include "Scanner.h"
#include <unistd.h>
//#include <stdlib.h>
//#include "y.tab.h"



//declare the global variables 
int builtin;
int bicmd;
char *bistr;
char *bistr2;


// ---- alias globle variables ----

struct alias aliastab[MAX];
char *aliasname;
char *aliastr;
int aliasLoop;				//return 1 is loop, 0 is not loop
int aliasNumb;



int CMD;

int main(void) 
{
	shell_init();
	while(1){
		printPrompt();

		//This calls yyparse()
		CMD = getCommand();
		switch(CMD)		//check if its a excutible command that follows all the rules.
		{
			//case BYE:		exit(1);
			case ERROR: 	recover_from_errors();
			case OK:		processCommand();
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
	
	printf("processCommand\n");
	printf("bicmd: %d", bicmd);
	printf("builtin: %d", builtin);

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
		case UNALIAS :
			deleteAlias(aliasname);
			break;
		case ALIASADD :
			addAlias(aliasname, aliastr);
			break;
		case EOL:
			break;
		default:	
				printf("unexpected error!");
				exit(1);

	}
}


// ----------------- functions for cd and env --------------

void changedir(char *s){
	if(chdir(s) == -1){
		printf("%s: ", s);
		printf("not a directory");
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


// ------------------ functions for alias -----------------------


int checkExistAlias(char* name)
{
	for(int i = 0; i < aliasNumb; i++)
	{
		if(strcmp(aliastab[i].alname,name) == 0)
			return i;
	}
	return -1;
}


int checkAliasLoop()
{
	return 0;
}		//1 is loop, 0 is not loop;


//need to update table in case there is a nested alias
void addAlias(char* name, char* str)
{

	// testing code
	char* temp;
	printf("name is %s, string is %s.\n", name, str);

	int aliasIndex = checkExistAlias(name);
	printf("aliasIndex is %d\n", aliasIndex);
	
	if(checkAliasLoop() == 0)
	{
		//printf("checkAliasLoop");
		//temp = noquoto(str);
		if(aliasIndex == (-1))			//if does not exist alias has same name
		{
			printf("aliasNumber is %d\n", aliasNumb);
			aliastab[aliasNumb].alname = name;
			aliastab[aliasNumb].alstr = str;

			aliasNumb++;
		}
		else
		{
			aliastab[aliasIndex].alname = name;
			aliastab[aliasIndex].alstr = str;
		}
	}
	printf("checkAliasLoop is end");
}

void deleteAlias(char* name)
{
	int aliasIndex = checkExistAlias(name);
	if(aliasIndex != -1)					// alias exist
	{
		//aliastab[aliasIndex].alname = '0';
		//aliastab[aliasIndex].alstr = '0';
		for(int i = aliasIndex; i < aliasNumb-1; i++ )
     	{	aliastab[i] = aliastab[i+1];	}
     	aliasNumb--;
	}
}

void showAlias()
{
	if(aliasNumb == 0)
		printf("alias table is empty!\n");
	else
	{
		for(int i = 0; i < aliasNumb; i++)
		{
			printf("alias %s=\"%s\"\n", aliastab[i].alname,aliastab[i].alstr);
		}
	}
}
/*
char* noquoto(char* s)
{
	char temp[100];
	int length = strlen(s);
	printf("length is %d\n", length);

	for(int i = 0; i < length-2; i++)
	{
		temp[i] = s[i+1];
		//printf("char %c\n", temp[i-1]);
	}
	printf("new string %s\n", temp);


	return temp;
}

*/








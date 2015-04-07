#include <stdio.h>
#include <string.h>
#include "Scanner.h"
#include <unistd.h>
#include "alias.c"
//#include <stdlib.h>
//#include "y.tab.h"



//declare the global variables 
int builtin;
int bicmd;
char *bistr;
char *bistr2;
int alProce;	

int CMD;
YY_BUFFER_STATE buffer;

int main(void) 
{
	/*
		char string[] = "\"sdfa\"\n String";
	    YY_BUFFER_STATE buffer = yy_scan_string(string);
	    yyparse();
	    yy_delete_buffer(buffer);
    return 0;
    */

	
	shell_init();
	while(1){

		//printf("alProce is %d\n", alProce);
		if(!alProce)
		{
			printPrompt();
		}	
		
		//printPrompt();

		CMD = getCommand();
		//yy_delete_buffer(buffer);	
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



void shell_init()
{	
	bicmd = 0;
	builtin = 0;
	aliasNumb = 0;
	inputd = 0;
	outputd = 0;
	alProce = 0; 			// does not process alias;
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


int getCommand()
{
	init_scanner_and_parser(); 
	//The value is 1 if parsing failed because of invalid input, i.e., input that contains a syntax error or that causes YYABORT to be invoked.
	


	if (yyparse())	
	{
		return (ERROR);
	}
	else
	{
		return (OK);		//The value returned by yyparse is 0 if parsing was successfu					
	}	
	
	
}

void init_scanner_and_parser(){
	bicmd = 0;
	builtin = 0;
	//set to null
	bistr = '\0';
	bistr2 = '\0';
	aliasname = '\0';
	aliastr = '\0';
	alORstr = 0;				//return 1 is alias, 0 is string
	inputd = 0;
	outputd = 0;
	//alProce; = 0; 				// does not process alias	
}



void recover_from_errors()
{ 	
	//Find out if error occurs in middle of command 
	//That is the command still has a “tail”
	//In this case you have to recover by “eating” 
	// the rest of the command.
	// To do this: use yylex() directly.
	printf("recover_from_errors\n");
	yy_delete_buffer(buffer);	
	//printf("delete buffer\n");
		
	printf("I have idea?\n");
	yyrestart(stdin);					//restart to stdin !!!
	//yy_delete_buffer();
	//yylex();
	//exit(1);
}

void processCommand()
{	
	//printf("processCommand\n");
	printf("bicmd: %d\n", bicmd);
	//printf("builtin: %d\n", builtin);

	if(builtin)
		do_it();
	else
	{
		execute_it();
	}
	
	//yy_delete_buffer(buffer);	
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
		case ALIASADDSTR :
			addAlias(aliasname, aliastr, 0);
			break;
		case ALIASADD :
			addAlias(aliasname, aliastr, 1);
			break;
		case EOL:
			break;
		default:	
			printf("unexpected error!");
				//exit(1);

	}
	if(alProce==1)
		alProce=0;
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

// ------------------- not builtin command --------------

void execute_it()
{
	if(alProce==1)			// extra command
		alProce=0;
	// Handle  command execution, pipelining, i/o redirection, and background processing. 
	// Utilize a command table whose components are plugged in during parsing by yacc. 

/* 
	 * Check Command Accessability and Executability
	*/
	if( ! Executable() ) {  
		//use access() system call with X_OK
		printf("Command not Found!\n");
		return;
	}
	else
	{
		printf("Command! Found!\n");
		if(builtin == 1)					//asume it can only be alias
		{
			processAlias(unknowStr);			// find the right command
			alProce = 1;						// now is processing on alias
			if(!alORstr)							// if alias is a string
			{

				char *temp = noquoto(aliastr);
				printf("is string %s\n",temp );
				buffer = yy_scan_string(temp);
			}
			else
			{
				printf("is alias %s\n",aliastr );
				buffer = yy_scan_string(aliastr);
			}
		}
		else
		{
			printf("error!\n");
			return;
		}
			printf("after excute_int\n");

	}
	/*
	 * Check io file existence in case of io-redirection.
	*/
	if( check_in_file()==ERROR ) {
		printf("Cann't read from ");
		return;
	}
	if( check_out_file()==ERROR ) {
		printf("Cann't write to ");
		return;
	}
	
	
	//Build up the pipeline (create and set up pipe end points (using pipe, dup) 
	//Process background
}

int Executable()
{
	// first check if it's an alias

	if(checkExistAlias(unknowStr)!= -1)
	{
		printf("in executable() Its alias");
		builtin = 1;
		return 1;	
	}
	else								// check whether it's a system call
	{

	}
				
	return 0;
} 

int check_in_file()
{
	return 0;
}

int check_out_file()
{
	return 0;
}


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




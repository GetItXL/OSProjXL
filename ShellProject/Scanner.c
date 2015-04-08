#include <stdio.h>
#include <string.h>
#include "Scanner.h"
#include <unistd.h>
#include "alias.c"
//#include <stdlib.h>
//#include "y.tab.h"
#include <sys/types.h>
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
int alProce;	
int     fd[2];
int *pipe_arr[10];
char    readbuffer[80];

//---- test ---
COMMAND comtab[MAXCMD];
int currcmd;
int currarg;
int numbCmd;
pid_t pid[MAXCMD]; 

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

    //For handling Ctrl + C
	signal(SIGINT, SIG_IGN);
	signal(SIGINT, signalHandling);
	//May need to use sigaction instead later.
	/** Move this to shell_init()*/

	
	shell_init();
	while(1){

		//printf("alProce is %d\n", alProce);
		if(!alProce)
		{
			printPrompt();
		}	
		
		CMD = getCommand();
		//yy_delete_buffer(buffer);	
		switch(CMD)		//check if its a excutible command that follows all the rules.
		{
			case ERROR: 	recover_from_errors();
							break;
			case OK:		processCommand();
							break;
			case BYE:		exit(1);
							break;
			default:		printf("Unexpected Error! 0x0\n");

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
	numbCmd = 0;			//
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
		return (understand_errors());
	}
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
	alORstr = 0;				//return 1 is alias, 0 is string
	inputd = 0;
	outputd = 0;
	numbCmd = 0;
	currcmd = 0; 				//not sure this.

	/** Avoid using nested for loop??
	  * may change comtab to be a pointer (COMMAND *comtab[]) instead
	  */
	
	// If use sizeof(array), then bus error
	int i, j;
	for(i = 0; i < MAXCMD; i++){
		//for(j = 0; j < sizeof(comtab[i].args); j++){
		for(j = 0; j < MAXARGS; j++){
			comtab[i].args[j] = NULL;
		}
		comtab[i].comName = NULL;
		comtab[i].countArgs = 0;
		comtab[i].infd = (STD);
		comtab[i].outfd = (STD);
	}

	//This will work in here for now. 
	//But for pipelining with multiple command, make sure to reset these in parser.y
	currcmd = 0;
	currarg = 1; //Do this here or in parser.y. Beacuse the first arg is reserved for the cmd
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
	char* temp = "nothing here";
	yyerror(temp);
	printf("I have idea?\n");
	yyrestart(stdin);					//restart to stdin !!!

	//yy_delete_buffer();
	//yylex();
	//exit(1);
}

int understand_errors()
{
	//if it's an error cause by alias
	
	//if it's an error syntex error;
	return (ERROR);

}

void processCommand()
{	
	//printf("processCommand\n");
	printf("bicmd: %d\n", bicmd);
	//printf("builtin: %d\n", builtin);

	if(builtin)
		do_it();
	else
	{		execute_it();	}
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

	// need to find the way
//	pid[0] = fork(); //create a child process
//	int     fd[2];
   	char    readbuffer[80];
	//Need to put this in a proper place
	comtab[currcmd].args[0] = comtab[currcmd].comName;

	if(alProce==1)			// extra command
		alProce=0;
	// Handle  command execution, pipelining, i/o redirection, and background processing. 
	// Utilize a command table whose components are plugged in during parsing by yacc. 


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
	
	/** Only tested with ls (no arg).
	  * Need to add pipeline, background, io redirection
	  * Need to modify parser to allow multiple args */
/*
	pid_t pid;
	int status;
	pid = fork(); //create a child process
	printf("I'm  on %d\n", pid);
	if(pid){ //If it's the parent process
		printf("I'm waiting on the child process\n");
		pid = wait(&status);
		printf("child process %d finished\n", pid);
	}
	else{	//if it's the child process, execute cmd

		//Debug
		int i;
		for(i = 0; i < sizeof(comtab[currcmd].args); i++){
			if(comtab[currcmd].args[i] != NULL)
				printf(" %s", comtab[currcmd].args[i]);
		}
		printf("\n");


		//Searches for the cmd automatically
		execvp(comtab[currcmd].comName, comtab[currcmd].args);
	}
*/
	//Build up the pipeline (create and set up pipe end points (using pipe, dup) 
	//Process background

	// pipeline implementation

	int status;
	//numbCmd++;

	pipe(fd);

	// need to keep pipeing, not executing.
	
	// pid = fork(); //create a child process

	printf("current cmd is %d, and numbCmd is %d \n",currcmd, numbCmd);
	for(int i = 0; i < numbCmd; i++)		//for each cmd;
	{
		
		//pipe(pipe_arr[i]);
		pid[i] = fork();
		
		printf("in the loop \n");
		

		printf("I'm  on %d\n", pid);
		switch(pid[i])
		{
			case 0:		commandPosition(i);
						break;
			default:	printf("I'm waiting on the child process\n");
						pid[i] = wait(&status);
						printf("child process %d finished\n", pid[0]);
						break;
		}

	}	
}

int executable()
{
	// first check if it's an alias

	if(checkExistAlias(unknowStr)!= -1)
	{
		printf("in executable() Its alias\n");
		builtin = 1;
		return (OK);	
	}
	else								// check whether it's a system call
	{
		return (OK);
	}
				
	return 0;
} 

void commandPosition(int cmd)
{
	// the pipeline here does not have logic, need fix issue.
	switch(whichCmd(cmd))
	{
		case FIRST:		printf("first command\n");
	//	If the parent wants to send data to the child, it should close fd0, and the child should close fd1.
						if(close(fd[0])==ERROR){}		//output 1, input 0
						if(dup2(fd[1],STDOUT_FILENO)!=1){}

						if(close(fd[1])==ERROR){}
						break;

		case LAST:		printf("last command\n");
	//	If the parent wants to receive data from the child, it should close fd1, and the child should close fd0. 
						if(close(fd[1])==ERROR){}
						if(dup2(fd[0],STDIN_FILENO)==ERROR){}
						
						if(close(fd[0])==ERROR){}
						break;

		case ONLY_ONE:	printf("only one command\n");
						in_redir();
						out_redir();
						//execvp(comtab[cmd].comName, comtab[cmd].args);
						

						break;
		default:		printf("middle command\n");
						if(dup2(fd[1],STDIN_FILENO)==ERROR){}
						if(dup2(fd[0],STDOUT_FILENO)==ERROR){}

						if(close(fd[0])==ERROR){}
						if(close(fd[1])==ERROR){}

						break;
	}
	printf("finished commandPosition\n");
			/* 
	 * Check Command Accessability and Executability
	*/
	if(executable() == (ERROR)) {  
		//use access() system call with X_OK
		printf("Command not Found!\n");
		return;
	}
	else
	{
		doCMD(cmd);
	}
	//numbCmd = 0;		//after the pipe, reset to 0;
	
}

void doCMD(int cmd)
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
			return;
		}
		else
		{
			//printf("that is not alias\n");
			printf("print name is %s, and cmd is %d\n",comtab[cmd].comName,cmd);
			execvp(comtab[cmd].comName, comtab[cmd].args);
			exit(0);
		}
}

int whichCmd(cmd)
{
	if(cmd == 0 && numbCmd == 1)
		return ONLY_ONE;
	else if(cmd == 0 && numbCmd != 1)
		return FIRST;
	else if(cmd == numbCmd-1)
		return LAST;
	else
		return -1;
}

void in_redir()
{
	if(comtab[currcmd].infd == STD)
	{
		printf("read from stdin\n");
	}	
	else if(comtab[currcmd].infd == FILE)
	{
		printf("read from file\n");
	}
	else			
	{
		printf("read from str\n");
	}
}
						
void out_redir()
{
	if(comtab[currcmd].outfd == STD)
	{
		printf("out to stdin\n");
	}	
	else if(comtab[currcmd].outfd == FILE)
	{
		printf("out to file\n");
	}
	else			
	{
		printf("out to str\n");
	}
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




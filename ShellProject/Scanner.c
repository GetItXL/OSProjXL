#include <stdio.h>
#include <string.h>
#include "Scanner.h"
#include <unistd.h>
#include "alias.c"
//#include <stdlib.h>
//#include "y.tab.h"
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

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
int   fd[2],fd_re[2]; 			// anything written to fd[1] can be read from fd[0]
int *pipe_arr[10];
char   readbuffer[80];

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
		
		//printPrompt();
		CMD = getCommand();
		yy_delete_buffer(buffer);	
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
	//yy_delete_buffer(buffer);	
	//printf("delete buffer\n");

	printf("I have idea?\n");
	//yyrestart(stdin);					//restart to stdin !!!
	//this may create a child process, cannot bye

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
	printf("-builtin----here\n");
	switch(bicmd){
		case CDX :
			changedir(bistr);
			break;
		case CDHOME :
			gohome();printf("-----here\n");
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
   
	//Need to put this in a proper place
//	comtab[currcmd].args[0] = comtab[currcmd].comName;
//	printf("...currcmd is %d.set my args[]table, current tabke name is %s \n",currcmd, comtab[currcmd].comName);
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

	//Build up the pipeline (create and set up pipe end points (using pipe, dup) 
	//Process background

	// pipeline implementation

	int status;
	//numbCmd++;
	
	status = pipe(fd);
		if(status == -1)
		{
			printf("state error!\n");
		}
	

	printf("current cmd is %d, and numbCmd is %d \n",currcmd, numbCmd);
	//for(int i = 0; i < numbCmd; i++)		//for each cmd;

	for(currcmd; currcmd < numbCmd; currcmd++)
	{
		comtab[currcmd].args[0] = comtab[currcmd].comName;
		
		printf("...currcmd is %d.set my args[]table, current tabke name is %s \n",currcmd, comtab[currcmd].comName);

		int pidnumb = pid[currcmd] = fork();

		printf("I'm  on fork # %d\n", pid[currcmd]);
		switch(pidnumb)
		{
			case -1:	printf("....fork error!\n");
						//break;
						exit(1);
			case 0:		//commandPosition(i);
						commandPosition(currcmd);
						//break;
						break;
			default:	
						printf("process #%d is on default\n", pid[currcmd]);
						break;
		}
	}	
	close(fd[0]); close(fd[1]); 
	if(waitpid(pid[numbCmd-1], &status, 0) != pid[numbCmd-1] )
		fprintf(stderr, "process %d exits with %d\n", pid[currcmd], WEXITSTATUS(status));
						
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
						// written to fd[1], read from fd[0];

						if(dup2(fd[1],STDOUT_FILENO)!=1){	printf("error line 2\n");}
						if(close(fd[0])==ERROR){ printf("error line 1\n");}		//output 1, input 0
						
						in_redir();

						break;

		case LAST:		printf("last command\n");
	//	If the parent wants to receive data from the child, it should close fd1, and the child should close fd0. 
						
						if(dup2(fd[0],STDIN_FILENO)==ERROR){}	
						if(close(fd[1])==ERROR){printf("error line 2\n");}
					
						 printf("i should outfd to \n");
						 printf("arg[0] is %s\n", comtab[cmd].args[0]);
						 printf("arg[1] is %s\n", comtab[cmd].args[1]);
						
						out_redir();
						break;

		case ONLY_ONE:	printf("only one command\n");
						in_redir();
						out_redir();
						printf("arg[0] is %s\n", comtab[cmd].args[0]);
						
						//execvp(comtab[cmd].comName, comtab[cmd].args);
						

						break;

		default:		printf("middle command\n");
						if(dup2(fd[1],STDIN_FILENO)==ERROR){}
						if(dup2(fd[0],STDOUT_FILENO)==ERROR){}

						if(close(fd[0])==ERROR){}
						if(close(fd[1])==ERROR){}

	}
	//printf("finished commandPosition: %d\n", cmd);
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

void doCMD(int cmd)
{
	//printf("Command! Found! %d \n", cmd);
		if(builtin == 1)					//asume it can only be alias
		{
			processAlias(unknowStr);			// find the right command
			alProce = 1;						// now is processing on alias
			if(!alORstr)							// if alias is a string
			{
				char *temp = noquoto(aliastr);
				printf("is string %s\n",temp );
				buffer = yy_scan_string(temp);
				free(temp);
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
		//	printf("print name is %s, and cmd is %d\n",comtab[cmd].comName,cmd);
			if( execvp(comtab[cmd].comName, comtab[cmd].args) < 0 ){
				printf("error executing %s, and %s\n", comtab[cmd].comName, strerror(errno));
			}
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
	char* temp = malloc(100);
	int length = strlen(s);
	printf("length is %d\n", length);

	int i;
	for( i = 0; i < length-2; i++)
	{
		temp[i] = s[i+1];
		//printf("char %c\n", temp[i-1]);
	}
	printf("new string %s\n", temp);


	return temp;
}




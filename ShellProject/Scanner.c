#include <stdio.h>
#include <string.h>
#include "Scanner.h"
#include <unistd.h>
#include "alias.c"
//#include <stdlib.h>
//#include "y.tab.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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

int bioutf = 0;
char *biOutfile = '\0';

int alProce;	
int   fd[MAXCMD][2]; 			// anything written to fd[1] can be read from fd[0]
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


int stdoutFD;

//Don't need to reset them for the next cmd right?
int newOutfd, newInfd;
//Used to store fd of in and out file



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
		
		/** To restore output stream to STDOUT after each cmd*/
		stdoutFD = dup(STDOUT);

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
		/** To restore output stream to STDOUT after each cmd*/
		dup2(stdoutFD, STDOUT);
		close(stdoutFD);

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

	bioutf = 0;
	biOutfile = '\0';

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
		//comtab[i].infd = (STD);
		//comtab[i].outfd = (STD);
		comtab[i].infd = 0;
		comtab[i].outfd = 0;
		comtab[i].infile = NULL;
		comtab[i].outfile = NULL;
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


	if(builtin){
		int fd, saveSTDOUT;
		if(bioutf){ //There is redirection
			
			if((fd = open(biOutfile, O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0){
				printf("Oops, fail to open file");
			}
			saveSTDOUT = dup(STDOUT); //Save stdout
			dup2(fd, STDOUT); //copy fd to stdou
			close(fd); //Release fd (no longer needed sinced copied to stdout)
			//redirect to output finished
		}
		do_it();

		/*
		if(bioutf){
			dup2(saveSTDOUT, STDOUT); //redirect output back to STDOUT after finished
			//close(fd);
			close(saveSTDOUT);
		}*/

	}
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
	if(alProce==1)			// extra command
		alProce=0;
	// Handle  command execution, pipelining, i/o redirection, and background processing. 
	// Utilize a command table whose components are plugged in during parsing by yacc. 


	/*
	 * Check io file existence in case of io-redirection.
	*/


	//Need these here instead of in the pipe loop so that any file error detected will cause the whole pipe cmd not executing
	if( check_in_file()==ERROR ) {
		printf("Cann't read from ");
		return;
	}
	if( check_out_file()==ERROR ) {
		printf("Cann't write to file");
		return;
	}
	/** I did the checking in in_redir() and out_redir() instead */

	//Build up the pipeline (create and set up pipe end points (using pipe, dup) 
	//Process background

	// pipeline implementation

	int status;
	//numbCmd++;
	int numbPip;

	//---------- set up for multiple pipe --------------- 
	// if(numbCmd == 1 || numbCmd == 2)
	// {
	// 	//status = pipe(fd[0]); 
	// 	//printf("......pipe is repeat\n");
	// }
	// else
	// {	
		
	// 	for(numbPip = 0; numbPip < numbCmd-1; numbPip++)
	// 	{
	// 		printf("i am pipeing #%d time\n", numbPip);
	// 		status = pipe(fd[numbPip]);
	// 		if(status == -1)
	// 			printf("state error!\n");
	// 	}
		
	// 	status = pipe(fd[0]);
	// 	status = pipe(fd[1]);
	// }

	
		
	int temp = numbCmd;

	printf("current cmd is %d, and numbCmd is %d \n",currcmd, numbCmd);
	//for(int i = 0; i < numbCmd; i++)		//for each cmd;

	for(currcmd; currcmd < numbCmd; currcmd++)
	{
		if((numbCmd == 1 && currcmd == 0)||(numbCmd == 2 && currcmd == 0))
		{
			status = pipe(fd[0]); printf("......pipe is repeat\n");
		}
		else if(currcmd < numbCmd-1)
		{
			status = pipe(fd[currcmd]);printf("pipe is repeat %d\n", currcmd);
		}

		comtab[currcmd].args[0] = comtab[currcmd].comName;
		
		printf("...currcmd is %d.set my args[]table, current tabke name is %s \n",currcmd, comtab[currcmd].comName);

		int pidnumb = pid[currcmd] = fork();

		printf("I'm  on fork # %d\n", pid[currcmd]);
		switch(pidnumb)
		{
			case -1:	printf("....fork error!\n");
						//break;
						exit(1);
			case 0:		commandPosition(currcmd);						//break;

						break;
			default:	
						printf("process #%d is on default\n", pid[currcmd]);
						if(currcmd!= 0 && numbCmd > 2 && currcmd != numbCmd)
						{
							printf("close pipe in the parent loop? %d \n", currcmd);
							if(close(fd[currcmd-1][1]) < 0){printf("error line 2\n");}
							if(close(fd[currcmd-1][0]) < 0){printf("error ssline 2\n");}
						}
						break;
		}
	}	

/*

waitpid can be either blocking or non-blocking:
If options is 0, then it is blocking
If options is WNOHANG, then is it non-blocking

*/
	if(numbCmd != 1 && numbCmd !=2 )
	{	printf("---close pipe now!!\n");
		
		for(numbPip = 0; numbPip < numbCmd-1; numbPip++)
		{
			close(fd[numbPip][0]); close(fd[numbPip][1]); 
			printf("close pipe #%d\n",numbPip);
		}	
		printf("wait for process #%d\n",currcmd);

		//waitpid(pid[0], &status, 0);
		//printf("wait for process 2 #%d\n",currcmd);
		if(waitpid(pid[currcmd-1], &status, 0) != pid[currcmd-1] )
			fprintf(stderr, "process %d exits with %d\n", pid[currcmd-1], WEXITSTATUS(status));
	  
	 }
	 else
	 {
	 	close(fd[0][0]); close(fd[0][1]); 
	 	waitpid(pid[currcmd-1], &status, 0);
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
						// written to fd[1], read from fd[0];

						if(dup2(fd[0][1],STDOUT_FILENO) < 0){		
							printf("first error line 2\n");}
						if(close(fd[0][0]) < 0){ 					//closing pipe read
							printf("first error line 1\n");}		//output 1, input 0
						if(close(fd[0][1]) < 0){ 					//closing pipe read
							printf("first error line 1\n");}		//output 1, input 0
						
						//in_redir();
						break;

		case LAST:		printf("last command, and the pipe is #%d\n", currcmd);
	//	If the parent wants to receive data from the child, it should close fd1, and the child should close fd0. 
						if(numbCmd == 1 )
						{
							if(dup2(fd[0][0],STDIN_FILENO) < 0){printf("last error line 1\n");	}
							if(close(fd[0][1]) < 0){printf("last error line 2\n");}
							if(close(fd[0][0]) < 0){printf("last error line 2\n");}
						}
						else
						{
							printf("last command, and reading the pipe frpm %d\n", currcmd); 
							if(dup2(fd[currcmd-1][0],STDIN_FILENO) < 0){ printf("error line \n");}
							
							if(close(fd[currcmd-1][1]) < 0){printf("error line 2\n");}
							if(close(fd[currcmd-1][0]) < 0){printf("error ssline 2\n");}
					
						}
						
						 printf("arg[0] is %s\n", comtab[cmd].args[0]);
						 printf("arg[1] is %s\n", comtab[cmd].args[1]);

						 out_redir(cmd);
			
					//	exit(0);
						break;

		case ONLY_ONE:	printf("only one command\n");
						//in_redir();
						out_redir(cmd);
						printf("arg[0] is %s\n", comtab[cmd].args[0]);
						
						//execvp(comtab[cmd].comName, comtab[cmd].args);
						

						break;

		default:		printf("middle command, and the pipe is #%d\n", currcmd);
						// fd[0][0] read			//fd[1][0] write
						if(dup2(fd[currcmd-1][0],STDIN_FILENO) < 0){printf("midd line 2\n");}
						if(dup2(fd[currcmd][1],STDOUT_FILENO) < 0){printf("mds line 2\n");}

						// curr = 1
						// close all the pipe
						if(close(fd[currcmd][0]) < 0){printf("sdfsa line 2\n");}
						if(close(fd[currcmd-1][1]) < 0){printf("errsdfor line 2\n");}
						if(close(fd[currcmd][1]) < 0){printf("sdfsa line 2\n");}
						if(close(fd[currcmd-1][0]) < 0){printf("errsdfor line 2\n");}
						

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


//newOutfd is visible to this method, so don't need pass arg					
void out_redir(cmd)
{
	//Can also access with comtab[numbCmd-1]
	if(comtab[cmd].outfd == 1){
		dup2(newOutfd, STDOUT);
		close(newOutfd);
	}
	/*
	//If has output redirection
	if(comtab[cmd].outfd == 1){
		int fd, saveSTDOUT;
		if((fd = open(comtab[cmd].outfile, O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0){
			printf("Oops, failed to open file");
		}
		//saveSTDOUT = dup(STDOUT); //Save stdout
		dup2(fd, STDOUT); //copy fd to stdou
		close(fd); //Release fd (no longer needed sinced copied to stdout)
		*/
	
}

int check_in_file()
{
	return 0;
}

int check_out_file()
{
	//Only check file existence if there is io redirection
	//numbCmd -1 = last cmd
	if(comtab[numbCmd-1].outfd == 1){
		//Created the outfile here
		if((newOutfd = open(comtab[numbCmd-1].outfile, O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0)
			return ERROR;
	}

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




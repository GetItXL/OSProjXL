#include <stdio.h>
#include <string.h>
#include "Scanner.h"
#include <unistd.h>
#include "alias.c"
#include <stdlib.h>
//#include "y.tab.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>


#include <glob.h>



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
int biinf = 0;
char *biInfile = '\0';

int alProce;	
int   fd[MAXCMD][2]; 			// anything written to fd[1] can be read from fd[0]

int errToSTDOUT = 0;
int errToFile = 0;
char* errFile = '\0';


//---- test ---
COMMAND comtab[MAXCMD];
int currcmd;
int currarg;
int numbCmd;
pid_t pid[MAXCMD]; 

int CMD;
YY_BUFFER_STATE buffer;
int amp;
int envCmd;
char* envValue;

//Used to save STDOUT so that when the io redirection cmd is finished, we can direct the output back to STDOUT
int saveSTDOUT;
int saveSTDIN;
int saveSTDERR;

//Don't need to reset them for the next cmd right?
int newOutfd, newInfd, errfd;
//Used to store fd of in and out file



int main(void) 
{

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
		saveSTDOUT = dup(STDOUT);
		saveSTDERR = dup(STDERR);

		CMD = getCommand();
		yy_delete_buffer(buffer);	
		switch(CMD)		//check if its a excutible command that follows all the rules.
		{
			case ERROR: 	recover_from_errors();
							break;
			case OK:		processCommand();
							break;
			case BYE:		exit(0);
							break;
			default:		printf("Unexpected Error! 0x0\n");

		}
		/** To restore output stream to STDOUT after each cmd*/
		dup2(saveSTDOUT, STDOUT);
		close(saveSTDOUT);
		//Need to do this to STDIN as well?? do I?
		//Did this for builtin cmd. But maybe don't need it for nonbuiltin
		dup2(saveSTDERR, STDERR);
		close(saveSTDERR);

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
	numbCmd = 0;			// numb of cmd in each command
	amp = 0;
	envCmd = 0;
	errToSTDOUT = 0;
	errToFile = 0;
	errFile = '\0';
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
	biinf = 0;
	biInfile = '\0';
	envCmd = 0;
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
	amp = 0;
	errToSTDOUT = 0;
	errToFile = 0;
	errFile = '\0';
}



void recover_from_errors()
{ 	
	//Find out if error occurs in middle of command 
	//That is the command still has a “tail”
	//In this case you have to recover by “eating” 
	// the rest of the command.
	// To do this: use yylex() directly.
	printf("recover_from_errors\n");

	printf("I have idea?\n");
	//yyrestart(stdin);					//restart to stdin !!!
	//this may create a child process, cannot bye
	//yylex();
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
	printf("contains amp?? %d\n",amp);

	if(builtin){
		int fd;
		if(bioutf){ //There is redirection
			if(bioutf == 2){ // append
				//For appending, if the file does not exist, it creates one also
				fd = open(biOutfile, O_CREAT | O_APPEND | O_WRONLY, 0644);
			}
			else{ //not appending = overwrite
				fd = open(biOutfile, O_CREAT | O_TRUNC | O_WRONLY, 0644);
			}

			/*
			if((fd = open(biOutfile, O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0){
				printf("Oops, fail to open file\n");
				return;
			}*/

			if(fd < 0){
				printf("Oops, fail to open file\n");
				return;
			}	

			dup2(fd, STDOUT); //copy fd to stdou
			close(fd); //Release fd (no longer needed sinced copied to stdout)
			//redirect to output finished
			//STDOUT is redirected at the end of each cmd in main();
		}

		//If there is input redirection
		if(biinf){
			if((fd = open(biInfile, O_RDONLY, 0600)) < 0){
				printf("Oops, you can't open this file\n");
				return; //Need this?
			}
			saveSTDIN = dup(STDIN);
				
			dup2(fd, STDIN);
			close(fd);
		}

		do_it();

		//Redirection
		if(biinf){
			dup2(saveSTDIN, STDIN);
			close(saveSTDIN);
		}
	}
	else
	{	execute_it();	}
}

void do_it(){
	switch(bicmd){
		printf("bistr is %s\n", bistr); 
		case CDX :
			changedir(bistr);
			break;
		case CDHOME :
			gohome();
			printf("im in CD home~~~\n");
			break;
		case PRINTENV :
			printEnv();
			break;
		case UNSETENV :
			unsetenv(bistr);
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
	{
		alProce=0;
		yyrestart(stdin);	
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
	// char* temp = "HOME";
	// char* value = getenv(temp);
	// printf("gohome here is %s\n",value);
	// chdir(value);
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

	//Need these here instead of in the pipe loop so that any file error detected will cause the whole pipe cmd not executing
	if( check_in_file()==ERROR ) {
		printf("Oops! You can't open the file! D:\n");
		return;
	}
	if( check_out_file()==ERROR ) {
		printf("Oops! You can't open the file! D:\n");
		return;
	}
	/** I did the checking in in_redir() and out_redir() instead */

	//Build up the pipeline (create and set up pipe end points (using pipe, dup) 
	//Process background

	int status;
	int numbPip;
	int temp = numbCmd;

	printf("current cmd is %d, and numbCmd is %d \n",currcmd, numbCmd);
	//for(int i = 0; i < numbCmd; i++)		//for each cmd;

	for(currcmd; currcmd < numbCmd; currcmd++)
	{
		//---------- set up for multiple pipe --------------- 

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
		if(amp == 0)
		{
			if(waitpid(pid[currcmd-1], &status, 0) != pid[currcmd-1] )
				fprintf(stderr, "process %d exits with %d\n", pid[currcmd-1], WEXITSTATUS(status));
		}
		else
		{
			waitpid(pid[currcmd-1], &status, WNOHANG);
		}
	 }
	 else
	 {
	 	close(fd[0][0]); close(fd[0][1]); 
	 	if(amp == 0)	
	 		waitpid(pid[currcmd-1], &status, 0);
	 	else
	 	{	
	 		waitpid(pid[currcmd-1], &status, WNOHANG);
	 		printf("run in the background.\n" );
	 	}
	 	
	 }
	  
					
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
						
						in_redir(cmd);
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

						 if(errToSTDOUT){
						 	err_to_stdout();
						 }
						 else if(errToFile){
						 	err_to_file();
						 }

			
					//	exit(0);
						break;

		case ONLY_ONE:	printf("only one command\n");
						in_redir(cmd);
						out_redir(cmd);
						printf("arg[0] is %s\n", comtab[cmd].args[0]);
						printf("arg[1] is %s\n", comtab[cmd].args[1]);


						if(errToSTDOUT){
						 	err_to_stdout();
						 }
						 else if(errToFile){
						 	err_to_file();
						 }
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

	 if(envCmd == 1)					//first check if it is an env variable
	{
		alProce = 2;
		printf("assume it is an env variable, %s\n", envValue);
		buffer = yy_scan_string(addEOL(envValue));
		return;
	}

	if(executable() == (ERROR)) {  
		//use access() system call with X_OK
		printf("Command not Found!\n");
		kill(getpid(), SIGTERM);
		return;
	}
	else
		doCMD(cmd);
		//after the pipe, reset to 0;
	
}

int executable()
{
	// first check if it's an alias
	printf("unknowStr is %s/n",unknowStr);
	if(checkExistAlias(unknowStr)!= -1)
	{
		printf("in executable() Its alias\n");
		builtin = 1;
		return (OK);	
	}
	else 								// check whether it's a system call, but how?
	{
		envValue = getenv(unknowStr);
		printf("value is %s\n", envValue);

		if(envValue != 0)
		{
			envCmd = 1;
			printf("alORstr is %d \n", alORstr);
			if(*envValue !='"')
				alORstr = 1;
			printf("%s is an envCmd, alORstr is %d \n", unknowStr, alORstr);
			alProce = 2;
			return (OK);
		}	

		//Check if cmd exists

		if(checkSystemCall() == 0){
			return ERROR;
		}
		else{
			printf("cmd found!\n");
			return (OK);
		}
	}
			
} 

void doCMD(int cmd)
{
	//printf("Command! Found! %d \n", cmd);
		if(builtin == 1)										/* first check if it is an alias */
		{
			processAlias(unknowStr);							// find the right command
			alProce = 1;										// now is processing on alias
			
			char *temp = noquoto(aliastr);

			printf("no quoto alias value is  %s\n",temp );
			buffer = yy_scan_string(temp);

			return;
		}
		else if(envCmd == 1)									/*  check if it is an enviroment vairable */
		{
			alProce = 2;
			
			printf("envCmd is a string \n" );
			char *temp = noquoto(envValue);
			buffer = yy_scan_string(temp);
	
			return;
		}	//printf("that is not alias\n");	
		else													/* other command */
		{
			printf("print name is %s, and cmd is %d\n",comtab[cmd].comName,cmd);
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

void err_to_stdout(){

	//Whatever in STDOUT now, put it in STDERR
	printf("Redirect err to current stdout\n");
	int currentSTDOUT = dup(STDOUT);
	dup2(currentSTDOUT, STDERR);
	close(currentSTDOUT);
}

void err_to_file(){
	printf("Redirect err to file\n");
	dup2(errfd, STDERR);
	close(errfd);
}

void in_redir(cmd)
{
	//If user specified input redirection
	if(comtab[cmd].infd == 1){

		dup2(newInfd, STDIN);
		close(newInfd);
	}
}
//Pipeline: When is input returned???

//newOutfd is visible to this method, so don't need pass arg					
void out_redir(cmd)
{
	//Can also access with comtab[numbCmd-1]
	if(comtab[cmd].outfd){
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
	//Only check file if "<" appears in the cmd
	if(comtab[0].infd == 1){
		//open infile here
		if((newInfd = open(comtab[0].infile, O_RDONLY, 0600)) < 0){ //0600?
			return ERROR;
		}
	}
	return 0;
}

int check_out_file()
{
	//Only check file existence if there is io redirection
	//numbCmd -1 = last cmd
	if(comtab[numbCmd-1].outfd){
		//Created the outfile here
		//If append
		if(comtab[numbCmd-1].outfd == 2){ //append
			newOutfd = open(comtab[numbCmd-1].outfile, O_CREAT | O_APPEND | O_WRONLY, 0644);
		}
		else{ //do not append
			newOutfd = open(comtab[numbCmd-1].outfile, O_CREAT | O_TRUNC | O_WRONLY, 0644);
		}		

		//if((newOutfd = open(comtab[numbCmd-1].outfile, O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0)
		//	return ERROR;

		if(newOutfd < 0)
			return ERROR;
	}

	//Checks error redirect file
	if(errToFile){
		if((errfd = open(errFile, O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0){
			return ERROR;
		}
	}


	return 0;
}

char* noquoto(char* s)
{
	char* temp = malloc(sizeof(s));
	if(envCmd != 1)
		temp = malloc(100);

	int length = strlen(s);
	
	if(!alORstr)
	{
		printf("length is %d\n", length);
		int i;
		for( i = 0; i < length-2; i++)
			temp[i] = s[i+1];
	}
	else
		strncpy(temp, s, length);

	strcat(temp, "\n");			// if there is a bug need to check here.
	printf("new string is %s\n", temp);
	// char* some = temp;
	// free(temp);
	return temp;
}


char* addEOL(char* s)
{
	char* temp = malloc(100);
	int length = strlen(s);
	strncpy(temp, s, length);
	strcat(temp, "\n");			// if there is a bug need to check here.
	printf("new addEOL string %s\n", temp);

	return temp;
}


//Check if it's system cmd
int checkSystemCall(){


	//currcmd

	char *home = strdup(getenv("HOME"));
	char *path = strdup(getenv("PATH"));

	char *pathToken;
	pathToken = strtok(path, ":"); 
	printf("pathtoken: %s\n", pathToken);


	char *fullpath = malloc(sizeof(pathToken) + 50);
	strcpy(fullpath, pathToken); //copy pathToken to fullpath
	strcat(fullpath, "/");
	strcat(fullpath, comtab[currcmd].comName);

	printf("fullpath: %s\n", fullpath);

	int ac;
	ac = access(fullpath, X_OK);
	printf("access result: %d\n", ac);
	if(ac == 0)
		return 1;
	

	fullpath = NULL;
	//free(fullpath);
	//Not freeing may cause error later???

	while((pathToken = strtok(NULL, ":")) != NULL){

		//printf("pathtoken: %s\n", pathToken);


		fullpath = malloc(sizeof(pathToken) + 50);
		strcpy(fullpath, pathToken);
		strcat(fullpath, "/");
		strcat(fullpath, comtab[currcmd].comName);
		printf("fullpath: %s\n", fullpath);
		//free(fullpath);
		
		ac = access(fullpath, X_OK);
		printf("access result: %d\n", ac);
		if(ac == 0)
			return 1;



		fullpath = NULL;

	}

	char *homeToken;
	homeToken = strtok(home, ":"); 
	printf("homeToken: %s\n", homeToken);


	char *fullhpath = malloc(sizeof(homeToken) + 50);
	strcpy(fullhpath, homeToken);
	//printf("Added slash: %s\n", pathToken);
	strcat(fullhpath, "/");
	strcat(fullhpath, comtab[currcmd].comName);

	printf("fullhpath: %s\n", fullhpath);

	int ach;
	ach = access(fullhpath, X_OK);
	printf("access result: %d\n", ach);
	if(ach == 0)
		return 1;


	fullhpath = NULL;
	//free(fullpath);
	//Not freeing may cause error later???

	while((homeToken = strtok(NULL, ":")) != NULL){

		//printf("pathtoken: %s\n", homeToken);


		fullhpath = malloc(sizeof(homeToken) + 50);
		strcpy(fullhpath, homeToken);
		strcat(fullhpath, "/");
		strcat(fullhpath, comtab[currcmd].comName);
		printf("fullhpath: %s\n", fullhpath);
		//free(fullpath);
		
		ach = access(fullhpath, X_OK);
		printf("access result: %d\n", ac);
		if(ach == 0)
			return 1;


		fullpath = NULL;

	}


	//free(fullpath);
	return 0;

}




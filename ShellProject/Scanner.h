#ifndef SCANNER_H_
#define SCANNER_H_

#include <stdio.h>
#include "y.tab.h"

#define ERROR 			1
#define OK				0
#define SYSTEMERR		-1
#define ALIASCMD		2


#define MAX 			100
#define MAXARGS			300
#define MAXCMD			100

#define CDHOME 			350
#define CDX 			351

#define ALIASADD 		352
#define ALIASADDSTR 	353	

#define FIRST			1
#define LAST			2
#define ONLY_ONE		3

// -------- for I/O implimentation ------

#define STD 			0
#define STR 			1
#define FILE 			2


#define STDIN 			0
#define STDOUT 			1
#define STDERR 			2

//#define LOGOUT 352
//You can either define it here or include "y.tab.h" which has the BYE definition already

/*
typedef struct comargs {
	char* args[MAXARGS];
} ARGTAB;*/


// command line structure
typedef struct com {
	char* comName;					//name of the command
	int countArgs;					//count of its arguments
	//ARGTAB *atptr;				//pointer to a list of null terminated arguments
	char *args[MAXARGS];
	int infd;						//Flag. infd = 0 means no in redirection				
	int outfd;						//Flag. outfd = 0 means no out redirection, 1 means has output redirection. 2 means append instead of overwrite
	char *infile;					//Directory for input redirection
	char *outfile;					//Directory for output redirection
} COMMAND;


/* alias structure */
struct alias {
	//int used;						//used = 1; new = 0 for another name of the alias reference to this alias 
	int refalias;					// 0 refer to a string, 1 refer to an alias
	char *alname;
	char *alstr;
};


void changedir(char*);
void print(void);
void shell_init();
void printPrompt();
int getCommand();

void init_scanner_and_parser();
void recover_from_errors();
int understand_errors();

void processCommand();
void do_it();
void gohome();
void printEnv();

// ------------ other command function ------------

void execute_it();
int checkSystemCall();
int check_in_file();
int check_out_file();

char* noquoto(char*);				// no quoto
int executable();
void doCMD(int);
char* addEOL(char*);

// ------------------ pipeline functions ------------

void commandPosition(int cmd);				//for pipeline
int whichCmd(int cmd);
void in_redir(int cmd);
void out_redir(int cmd);

//------- function for alias -----------

int checkExistAlias(char*);					// -1 does not exist, else return index position;
int checkAliasLoop(char*, char*, int);				//0 is loop, 1 is not loop;
void addAlias(char*, char*, int);	
void deleteAlias(char*);
void showAlias();							//display all alias in tab
void processAlias(char*);					//check if input is an alias
//int alias_input(char*);
//int helper(char*, int, int);

// ---------------------IO---------
void err_to_stdout();
void err_to_file();


// --------------------------------



extern COMMAND comtab[MAXCMD];
extern int yylex();
extern int yylineno;
extern char* yytext;
extern int yyparse();
extern char **environ;
extern int builtin;
extern int bicmd;
extern char *bistr;
extern char *bistr2;

extern int bioutf; //0 = no outfile redirection. 1 = has redirection. 2 means append instead of overwrite
extern char *biOutfile;
extern int biinf;	//0 = no input redirection. 1 = has input redirection
extern char *biInfile;

extern int envCmd;
extern char* envValue;

// --------------- IO Redirect ----------------
extern int errToSTDOUT;
extern int errToFile;
extern char *errFile;

// ---------------- for pipline ---------------

extern int currcmd;					//current cmd index position in the comtab;
extern int currarg;
extern int numbCmd;					//to record how many number of cmd at once.


// --------- global variables for alias ------------

extern struct alias aliastab[MAX];	// table to store alias 
extern int alORstr;					//return 1 is alias, 0 is string
extern int aliasNumb;				// count number of alias in tab
extern char *aliasname;
extern char *aliastr;
extern int alProce;					// 0 is not alias processing, 1 is processing

// ------------ something else ------------

extern int inputd;					// 0 stdin, 1 string, 2 files
extern int outputd;					// 0 stdout, 1 string, 2 files
extern char *unknowStr;
extern void yyerror(char *);
extern int amp;						// check whether the command contains & at the end
									// 0 null, 1 contain

// ------------- read string as command ----------

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

#endif




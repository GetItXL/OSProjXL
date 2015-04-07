#ifndef SCANNER_H_
#define SCANNER_H_

#include <stdio.h>
#include "y.tab.h"

#define ERROR 			1
#define OK				0
#define ALIASCMD		2

#define MAX 			100
#define MAXARGS			300
#define MAXCMD			100

#define CDHOME 			350
#define CDX 			351

#define ALIASADD 		352
#define ALIASADDSTR 	353	
//#define LOGOUT 352
//You can either define it here or include "y.tab.h" which has the BYE definition already

typedef struct comargs {
	char* args[MAXARGS];
} ARGTAB;


// command line structure
typedef struct com {
	char* comName;				//name of the command
	int countArgs;				//count of its arguments
	ARGTAB *atptr;				//pointer to a list of null terminated arguments
	int infd;					//intput file name
	int outfd;					//output file name
} COMMAND;


/* alias structure */
struct alias {
	//int used;			//used = 1; new = 0 for another name of the alias reference to this alias 
	int refalias;		// 0 refer to a string, 1 refer to an alias
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
void processCommand();
void do_it();
void gohome();
void printEnv();

// ------------ other command function ------------
void execute_it();
int check_in_file();
int check_out_file();
int executable();

//------- function for alias -----------

int checkExistAlias(char*);			// -1 does not exist, else return index position;
int checkAliasLoop(char*, int);				//0 is loop, 1 is not loop;
void addAlias(char*, char*, int);	
void deleteAlias(char*);
void showAlias();					//display all alias in tab
char* noquoto(char*);
void processAlias(char*);			//check if input is an alias
int alias_input(char*);
int helper(char*, int, int);

// -------------------------------------

extern COMMAND comtab[];
extern int yylex();
extern int yylineno;
extern char* yytext;
extern int yyparse();
extern char **environ;
extern int builtin;
extern int bicmd;
extern char *bistr;
extern char *bistr2;
extern int currcmd;

// --------- global variables for alias ------------
extern struct alias aliastab[MAX];	// table to store alias 
extern int aliasLoop;				//return 1 is loop, 0 is not loop
extern int aliasNumb;				// count number of alias in tab
extern char *aliasname;
extern char *aliastr;

// ------------ something else ------------

extern int inputd;					// 0 stdin, 1 string, 2 files
extern int outputd;					// 0 stdout, 1 string, 2 files
extern char *unknowStr;

// ------------- read string as command ----------
typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

#endif




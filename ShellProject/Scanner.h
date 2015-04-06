#ifndef SCANNER_H_
#define SCANNER_H_

#include <stdio.h>
#include "y.tab.h"

#define ERROR 			1
#define OK				0
#define ALIASCMD		2

#define MAX 			100

#define CDHOME 			350
#define CDX 			351

#define ALIASADD 		352
#define ALIASADDSTR 	353	
//#define LOGOUT 352
//You can either define it here or include "y.tab.h" which has the BYE definition already


/* alias structure */
struct alias {
	int used;			//used = 1; new = 0
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

//------- function for alias -----------

int checkExistAlias(char*);		// -1 does not exist, else return index position;
int checkAliasLoop();				//0 is loop, 1 is not loop;
void addAlias(char*, char*);	
void deleteAlias(char*);
void showAlias();					//display all alias in tab
//char* noquoto(char*);
void processAlias(char*);			//check if there is a alias
int alias_input(int cmd);
// -------------------------------------

extern int yylex();
extern int yylineno;
extern char* yytext;
extern int yyparse();
extern char **environ;
extern int builtin;
extern int bicmd;
extern char *bistr;
extern char *bistr2;

// --------- global variables for alias ------------
extern struct alias aliastab[MAX];	// table to store alias 
extern int aliasLoop;				//return 1 is loop, 0 is not loop
extern int aliasNumb;				// count number of alias in tab
extern char *aliasname;
extern char *aliastr;
extern char *strAlias;

#endif




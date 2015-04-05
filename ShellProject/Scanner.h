#include <stdio.h>
#include "y.tab.h"
#define ERROR 	1
#define OK		0

#define CDHOME 350
#define CDX 351
//#define LOGOUT 352
//You can either define it here or include "y.tab.h" which has the BYE definition already


/* alias structure */
// struct alias {
// 	int used;			//used = 1; new = 0
// 	char *alname;
// 	char *alstr;
// };




void shell_init(void);



void printPrompt(void);
// {
// 	printf("shell < ");
// }

int getCommand();
// {


void recover_from_errors();

//{ printf("recover_from_errors");}

void processCommand();
//{printf("processCommand");}


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

extern int yylex();
extern int yylineno;
extern char* yytext;
extern int yyparse();

extern char **environ;

extern int builtin;
extern int bicmd;
extern char *bistr;
extern char *bistr2;
//extern struct alias aliastab[];	// table to store alias 







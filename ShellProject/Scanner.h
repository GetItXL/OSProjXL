#include <stdio.h>
#include "y.tab.h"
#define ERROR 	1
#define OK		0

#define CDHOME 350
#define CDX 351
//#define LOGOUT 352
//You can either define it here or include "y.tab.h" which has the BYE definition already




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


extern int builtin;
extern int bicmd;
extern char *bistr;







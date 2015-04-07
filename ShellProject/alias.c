#include <stdio.h>
#include <string.h>
#include "Scanner.h"
#include <unistd.h>



// ---- alias globle variables ----

struct alias aliastab[MAX];
char *aliasname;
char *aliastr;
int aliasLoop;				//return 1 is loop, 0 is not loop
int aliasNumb;
char *unknowStr;
int inputd;
int outputd;
char* cmd;


// ------------------ functions for alias -----------------------


int checkExistAlias(char* str)
{
	for(int i = 0; i < aliasNumb; i++)
	{
		if(strcmp(aliastab[i].alname,str) == 0)
		{
			return i;
		}	
	}
	return -1;
}

int helper(char *str, int refalias, int times)
{
	if(times > aliasNumb)										//base case
		return ERROR;

	if(refalias == 1)											// is an alias
	{
		for(int i = 0; i < aliasNumb; i++)
		{
			if(strcmp(aliastab[i].alname,str) == 0)				// if the string is in the alias table
			{													// recursively check for 
				if(helper(aliastab[i].alstr, aliastab[i].refalias, ++times))		
					return OK;
				else
					return ERROR;
			}
		}
		return (OK);
	}
	else
		return (OK);
}

int checkAliasLoop(char *str, int refalias)
{
	if(refalias == 1)											// is an alias
	{
		for(int i = 0; i < aliasNumb; i++)
		{
			if(strcmp(aliastab[i].alname,str) == 0)				// if my string is your name
			{													// recursively to check if your string is other's name
				if(helper(aliastab[i].alstr, aliastab[i].refalias, 1))		
					return OK;
				else
					return ERROR;
			}	
		}
		return (OK);
	}
	else
		return (OK);
}		//1 is loop, 0 is not loop;


//need to update table in case there is a nested alias
void addAlias(char* name, char* str, int refalias)
{
	//printf("name is %s, string is %s.\n", name, str);
	int aliasIndex = checkExistAlias(name);
	//printf("aliasIndex is %d\n", aliasIndex);
	
	if(checkAliasLoop(str, refalias) == 0)
	{
		//printf("checkAliasLoop");
		//temp = noquoto(str);
		if(aliasIndex == (-1))			//if does not exist alias has same name
		{
			//printf("aliasNumber is %d\n", aliasNumb);
			aliastab[aliasNumb].alname = name;
			aliastab[aliasNumb].alstr = str;

			aliasNumb++;
		}
		else
		{
			aliastab[aliasIndex].alstr = str;
		}

		if(refalias == 0)					//refer to a string 
		{
			aliastab[aliasNumb].refalias = 0;
		}
		else
		{
			aliastab[aliasNumb].refalias = 1;
		}
	}
	else
	{
		printf("Error! unable to add alias!\n");
	}
	//printf("checkAliasLoop is end");
}

void deleteAlias(char* name)
{
	int aliasIndex = checkExistAlias(name);
	if(aliasIndex != -1)					// alias exist
	{
		//aliastab[aliasIndex].alname = '0';
		//aliastab[aliasIndex].alstr = '0';
		for(int i = aliasIndex; i < aliasNumb-1; i++ )
     	{	aliastab[i] = aliastab[i+1];	}
     	aliasNumb--;
	}
	else
		printf("Error! unable to delete alias!\n");
}

void showAlias()
{
	if(aliasNumb == 0)
		printf("alias table is empty!\n");
	else
	{
		//printf("aliasNumb is %d\n", aliasNumb);
		for(int i = 0; i < aliasNumb; i++)
		{
			//printf("alias %s=\"%s\"\n", aliastab[i].alname,aliastab[i].alstr);
			printf("alias %s=%s\n", aliastab[i].alname,aliastab[i].alstr);
		}
	}
}

void processAlias(char* unknowStr)
{
	
	//printf("word in processALias.c %s\n", unknowStr);
	// 	//cmd is from aliastab
	int i = checkExistAlias(unknowStr);
	if(i > -1)
	{	
		cmd = aliastab[i].alstr;	
		aliastr = aliastab[i].alstr;
	}
	else
		printf("error in the alias!!!\n");
	
	while (1) {
		if (alias_input(cmd) > -1)	
		{	printf("true!\n");	}// if command has alias, check alias table 				// and expand it
		else
		{
			printf("false!\n");
			break;
		}
			
	}

		// int cmd = checkExistAlias(unknowStr);
		// //printf("cmd is %d\n",cmd);
		
		// if(cmd == -1)
		// {
		// 	printf("unreganized input! \n");
		// 	//break;
		// }
		// else
		// {
		// 	inputd = 1;
		// 	printf("input is an alias : %s, and action : %s\n", aliastab[cmd].alname, aliastab[cmd].alstr);
		// 	// if command has alias, check alias table and expand it
			
		// }
			
}


int alias_input(char* cmd)
{
	
	//should return alias name corrsponding index value;

	printf("in the alias_input, cmd is %s \n", cmd);
	
	return -1;
}
/*
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

*/
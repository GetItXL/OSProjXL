#include <stdio.h>
#include <string.h>
#include "Scanner.h"
#include <unistd.h>



// ---- alias globle variables ----

struct alias aliastab[MAX];
char *aliasname;
char *aliastr;
int alORstr;				//return 1 is loop, 0 is not loop
int aliasNumb;
char *unknowStr;
int inputd;
int outputd;
char* cmd;

struct node {
  char* x;
  struct node *next;
};

// ------------------ functions for alias -----------------------


int checkExistAlias(char* str)
{
	int i;
	for( i = 0; i < aliasNumb; i++)
	{
		if(strcmp(aliastab[i].alname,str) == 0)
		{
			return i;
		}	
	}
	return -1;
}

int checkAliasLoop(char* name, char *str, int refalias)
{
	
	  if(refalias == 1)
	  {
			struct node *root = malloc( sizeof(struct node) );  
			struct node *checkexit;   
			root->next = malloc( sizeof(struct node) );
			root->x = name;
	
			checkexit = root->next;
			checkexit->x = str;
		//	printf("insert root %s in the linkedlist!\n", root->x);		// root is the value of the alias 
		//	printf("insert value %s in the linkedlist!\n", checkexit->x);

			checkexit->next = malloc( sizeof(struct node) );
			checkexit = checkexit->next;
			
			int i, temp;
			
			// need to fine whether there the value is the key in the alias table
			temp = checkExistAlias(str);
			if(temp == -1)								// if new value is not an alias name, good
				return OK;	
			else									// if the new value is exist in the linked list, not good
			{	
					// insert the alias name in the linkedlist
				while(temp!=-1)
				{
					checkexit->x = aliastab[temp].alstr;
					checkexit->next = malloc( sizeof(struct node) ); 

					
				//	printf("check is  %s in the linkedlist, and str is %s!\n",checkexit->x, str);
					temp = checkExistAlias(aliastab[temp].alstr);

					if(strcmp(checkexit->x ,str)== 0)
						return ERROR;
					if(strcmp(checkexit->x ,name)== 0)
						return ERROR;
					checkexit = checkexit->next;
				}	
			//	printf("temp is %d , and temp is  %s in the linkedlist!\n", temp, aliastab[temp].alstr);
					
			}
		//	printf("check if there is duplicate value\n");
			checkexit = root;
			while(checkexit!=NULL)
			{
				checkexit = root->next;
			 	free(root);
			 	root = checkexit;
			}	
		//		printf("finished checking alias table!, nothing error\n");
				return OK;
	  }
	  else
	  		return OK;
}		//1 is loop, 0 is not loop;


//need to update table in case there is a nested alias
void addAlias(char* name, char* str, int refalias)
{
	//printf("name is %s, string is %s.\n", name, str);
	int aliasIndex = checkExistAlias(name);
//	printf("aliasIndex is %d\n", aliasIndex);

	if(checkAliasLoop(name, str, refalias) == 0)
	{
		if(aliasIndex == (-1))			//if does not exist alias has same name
		{
			//printf("aliasNumber is %d\n", aliasNumb);
			aliastab[aliasNumb].alname = name;
			aliastab[aliasNumb].alstr = str;
			if(refalias == 0)					//refer to a string 
				aliastab[aliasNumb].refalias = 0;
			else
				aliastab[aliasNumb].refalias = 1;
			aliasNumb++;
		}
		else
		{
			aliastab[aliasIndex].alstr = str;
			if(refalias == 0)					//refer to a string 
				aliastab[aliasIndex].refalias = 0;
			else
				aliastab[aliasIndex].refalias = 1;	
		}
	}
	else
	{
		printf("Error! unable to add alias!\n");
	}
}

void deleteAlias(char* name)
{
	int aliasIndex = checkExistAlias(name);
	if(aliasIndex != -1)					// alias exist
	{
		int i;
		for(i = aliasIndex; i < aliasNumb-1; i++ )
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
		int i;
		for(i = 0; i < aliasNumb; i++)
		{
			printf("alias %s=%s\n", aliastab[i].alname,aliastab[i].alstr);
		}
	}
}

void processAlias(char* unknowStr)
{
	int i = checkExistAlias(unknowStr);
	if(i > -1)
	{	
		cmd = aliastab[i].alstr;	
		aliastr = aliastab[i].alstr;
		alORstr = aliastab[i].refalias;
	}
	else
	{
		printf("Error! cannot find alias!!!\n");
		return;
	}
}


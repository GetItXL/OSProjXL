# OSProjXL
Operating system project &lt;System Shell> using lex and yacc

===================== Makefile Instruction ====================
How to:
    Compile with “make”, run with ./Scanner. 
	Enter bye to exit the shell. 
	After exit, run command “make clean” to remove extra files.

===================== Contributors ============================
Contributors:
- Xixi Ruan
- Qianwen Ding

===================== Files ===================================
-Parser.y
	Contains Yacc grammar for processing tokens
-scanner.l
	Contains the lex code for token definition
-Scanner.h
	Contains the global/local variable and function declarations
-Scanner.c
	Contains main() and other necessary functions to execute builtin and nonbuiltin commands, io redirection, and pipeline.
-alias.c
	Contains the functions that implements the builtin command: alias
-makefile

==================== Working Functions =======================
Our shell project is similar to the korn shell. It can:
1. run buildin commands
	- setenv variableName value
		Sets environment variable “variableName” to value
	- printenv
		Prints a list of all environment variables in the form of variableName = value
	- unsetenv variableName
		Removes the binding of variable with “variableName”
	- cd
		Takes the user back to the home directory
	- cd directory
		Takes the user to the specified directory if exists
	- alias
		Prints out all the alias in the alias table
	- alias aliasName value
		Set aliasName with value
	- unalias aliasName
		Remove the alias with aliasName
	- bye
		exits the shell

2. run any non-builtin command that exists in the system. If after the shell searching through all the directories in PATH and HOME variables and could not find the non-builtin command, it returns error.
3. recognize arguments for the commands
4. read, input files
	- Redirects input of a command from a file
	- Redirects output of a command to a file
	- Redirect STDERR to screen or to a file
5. executes commands in pipeline
6. run command in the background.
7. perform wildcard matching
8. perform tilde expansion
9. expand environment variable of the form ${variable}
 
===================== Note About Syntax =========================
If you get syntax error, please check if your input matches the following format:

NOTE: PLEASE PAY ATTENTION TO THE  WHITESPACE !

word: any combination from {letter, number, or ./:~*?_- }
string: word surrounded by double quotes : {“word”}

syntax for built-in commands:
    alias
    alias word word        (for alias refer to another alias only)
    alias word string        (for assigning an alias with a value. The value has to be inside “”)
    unalias word
    printenv
    unsetenv word
    cd word
    bye

syntax for other commands:
    cmd [arg]* [| cmd [arg]* ] * [< file1] [ >[>] file2] [ 2 > file3 || 2>&1]
    
    |: pipe
    < input redirect 
    > output redirect (overwrite a file if a file already exist)
    >> output redirect (append to a file if a file already exist)
    *Note: for 2 > file3, there is whitespace b\w 2 and > and there is whitespace between > and file3. For 2>&1, there is no whitespace in between.

Wildcard Matching:                                       
	A * matches any string, including the null string. A ? matches any single character. The ‘.’ at the start of a filename or immediately following a ‘/’, and the character ‘/’ itself are matched explicitly.

Tilde Expansion:
	~username will be replaced with the home directory of user with username
	~/ and ~will be replaced with the home directory of the current user
     
                
            
        


		

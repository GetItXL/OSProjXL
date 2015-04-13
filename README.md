# OSProjXL
Operating system project &lt;System Shell> using lex and yacc

How to:
	Compile with “make”, run with ./Scanner. 
Enter bye to exist the project. 
After exist, run command “make clean” to remove extra files.

Contributors:
Xixi Ruan
Qianwen Ding

Accomplishments:

our shell project is very similar to the real shell. It can:
  run buildin commands
  setenv
  printenv
  unsetenv
  vairable
  cd
  alias
  unalias
  run any non-builtin command
  recognize arguments for the comman
  read, input files
  read pipe instruction
  run in the background.
  do wildcard matching
  expanse environment variable


Help:

For a syntax error please check your input match the format:
word: any combination from {character, number, or ./:~*?_ }
string: quote with word {“word”}

NOTE: PLEASE PAY ATTENTION WITH THE SPACE !
syntax for built-in commands:
	alias
	alias word word		(for alias refer to another alias only)
	alias word string		(for alias refer to others)
	unalias word
	printenv
	unsetenv name
	cd word
	bye
	
syntax for other commands:
	cmd [arg]* [| cmd [arg]* ] * [< file1] [ >[>] file2] [ 2> file3 || 2>&1]
	
	|: pipe
	< input file 
	> output file (replace contains)
	>> output file 

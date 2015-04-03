Scanner: lex.yy.c y.tab.c
	 gcc -g lex.yy.c y.tab.c -ll -o Scanner 

lex.yy.c: y.tab.c Scanner.l
	lex Scanner.l

y.tab.c: Parser.y
	yacc -d -t Parser.y

clean:
	rm Scanner
	rm lex.yy.c
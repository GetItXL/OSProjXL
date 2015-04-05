Scanner: Scanner.c Scanner.h lex.yy.c y.tab.c
	 cc lex.yy.c y.tab.c -o Scanner -ll 

lex.yy.c: y.tab.c Scanner.l
	 lex Scanner.l

y.tab.c: Parser.y
	 yacc -d Parser.y

clean:
	rm -f *.o lex.yy.c *.tab.* Scanner
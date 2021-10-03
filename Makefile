parser: syntax.tab.c lex
	gcc main.c syntax.tab.c tree.c -lfl -o parser
syntax.tab.c: syntax.y
	bison -d syntax.y
lex: lexical.l
	flex lexical.l
.PHONY:clean
clean:
	rm *.output
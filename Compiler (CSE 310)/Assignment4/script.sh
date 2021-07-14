yacc -d -y -v parser.y
g++ -w -c -o y.o y.tab.c
flex lexer.l		
g++ -w -c -o l.o lex.yy.c
g++ -o a.out y.o l.o -lfl

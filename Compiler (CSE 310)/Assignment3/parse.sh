bison -d -y -v parser.y
g++ -w -c -o y.o y.tab.c
flex lexer.l
g++ -w -c -o l.o lex.yy.c
g++ y.o l.o -lfl
./a.out input.c

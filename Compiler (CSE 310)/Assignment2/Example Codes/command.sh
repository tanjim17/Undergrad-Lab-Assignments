flex -o lexer.cpp lexer.l
g++ lexer.cpp -lfl -o lexer.out
./lexer.out input.txt

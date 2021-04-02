flex -o lexer.cpp lexer.l
g++ lexer.cpp -lfl -o lexer.out
./lexer.out sample_input5.txt

A compiler of a subset of C language was built in this course through four assignments. The last assignment i.e. *Assignment4* folder contains all the necessary
files including the grammar of the language. The compiler takes a C code and translates it into an 8086 assembly code.

**OS**: Linux

**Required packages**: g++, flex and bison

**How to compile**:

1. Download the *Assignment4* folder.
2. Run *script.sh*. All the necessary files for compilation will be created.
3. Run *compile.sh* and you will be prompted to enter a source file's path. Enter your source file's path (*Assignment4 folder contains a sample source file*).
Two assembly files named *code.asm* and *optimized_code.asm* will be created.
4. Run any of the assembly files in an 8086 emulator.

*Note*: You can run *script.sh* only once. You only need to run *compile.sh* every time you compile. You need to run *script.sh* again, if you change the compiler's
code (*SymbolTable.cpp, lexer.l or parser.y*).

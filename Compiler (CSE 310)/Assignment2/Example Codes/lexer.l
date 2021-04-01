%option noyywrap

%{
#include<bits/stdc++.h>
#include "SymbolTable.cpp"
using namespace std;

ofstream logout;
ofstream tokenout;

SymbolTable st(7);
int line_count;

void printKeywordToken() {
	string upperCaseKeyword = yytext;
	transform(upperCaseKeyword.begin(), upperCaseKeyword.end(), upperCaseKeyword.begin(), ::toupper);
	tokenout << "<" << upperCaseKeyword << "> ";
	logout << "Line no " << line_count << " TOKEN <" << upperCaseKeyword << "> Lexeme " << yytext << " found\n";
}

void printOperatorToken(string type) {
	tokenout << "<" + type + ", " << yytext << "> ";
	logout << "Line no " << line_count << " TOKEN <" + type + "> Lexeme " << yytext << " found\n";
}

char getEscapeSeqValue(char c) {
	switch(c) {
		case 'n':
			return '\n'; break;
		case 't':
			return '\t'; break;
		case '\\':
			return '\\'; break;
		case '\'':
			return '\''; break;
		case 'a':
			return '\a'; break;
		case 'f':
			return '\f'; break;
		case 'r':
			return '\r'; break;
		case 'b':
			return '\b'; break;
		case 'v':
			return '\v'; break;
		case '0':
			return '\0'; break;
		default:
			cout << "\nerror\n"; return 'x';
	}
}

%}

KEYWORD (if|else|for|while|do|break|int|char|float|double|void|return|switch|case|default|continue)
WHITESPACE [ \t\f\r\v]+ 
LETTER [a-zA-Z]
DIGIT [0-9]
ESCAPE_SEQUENCE \\[nt\\'afrbv0]

%%
{WHITESPACE} {}
[\n] {line_count ++;}
{KEYWORD} {printKeywordToken();}

[\+\-] {printOperatorToken("ADDOP");}

("++"|"--") {printOperatorToken("INCOP");}
[\*/%] {printOperatorToken("MULOP");}
("<"|"<="|">"|">="|"=="|"!=") {printOperatorToken("RELOP");}
"=" {printOperatorToken("ASSIGNOP");}
("&&"|"||") {printOperatorToken("LOGICOP");} //confused
"!" {printOperatorToken("NOT");}
"(" {printOperatorToken("LPAREN");}
")" {printOperatorToken("RPAREN");}
"{" {printOperatorToken("LCURL"); st.enterScope();}
"}" {printOperatorToken("RCURL"); st.exitScope();}
"[" {printOperatorToken("LTHIRD");}
"]" {printOperatorToken("RTHIRD");}
"," {printOperatorToken("COMMA");}
";" {printOperatorToken("SEMICOLON");}
	
{LETTER}+({LETTER}|{DIGIT}|_)* {
	tokenout << "<ID, " << yytext << "> ";
	logout << "Line no " << line_count << " TOKEN <ID> Lexeme " << yytext << " found\n";
	st.insert(yytext, "ID");
	st.printAllScopeTables(logout);
}

{DIGIT}+ {
	tokenout << "<CONST_INT, " << yytext << "> ";
	logout << "Line no " << line_count << " TOKEN <CONST_INT> Lexeme " << yytext << " found\n";
	st.insert(yytext, "CONST_INT");
	st.printAllScopeTables(logout);
}	

{DIGIT}+\.{DIGIT}+(E[+-]?{DIGIT}+)? {
	tokenout << "<CONST_FLOAT, " << yytext << "> ";
	logout << "Line no " << line_count << " TOKEN <CONST_FLOAT> Lexeme " << yytext << " found\n";
	st.insert(yytext, "CONST_FLOAT");
	st.printAllScopeTables(logout);
}

'({ESCAPE_SEQUENCE}|[ -~]{-}[\\'"])' {
	string temp = yytext;
	temp.erase(0,1);
	temp.pop_back();
	cout << temp << endl;
	char c;
	if(temp[0] == '\\') {
		c = getEscapeSeqValue(temp[1]);
	}
	else {
		c = temp[0];
	}
	cout << c;
	string token = string("<CONST_CHAR, ") + c + "> ";
	tokenout <<  token;
	logout << "Line no " << line_count << " TOKEN <CONST_CHAR> Lexeme " << yytext << " found --> " << token << "\n";
	st.insert(yytext, "CONST_CHAR"); 
	st.printAllScopeTables(logout);
}
%%

int main(int argc,char *argv[]){	
	if(argc!=2){
		printf("Please provide input file name and try again\n");
		return 0;
	}
	
	FILE *fin=fopen(argv[1],"r");
	if(fin==NULL){
		printf("Cannot open specified file\n");
		return 0;
	}
	
	logout.open("log.txt");
	tokenout.open("token.txt");
	
	yyin= fin;
	line_count = 1;
	yylex();
	fclose(yyin);
	tokenout.close();
	logout.close();
	return 0;
	/*
	
	
	*/
}

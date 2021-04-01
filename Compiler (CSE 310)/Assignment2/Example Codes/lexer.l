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

string replaceWithEscapeSeq(string s) {
	for(int i=0; i<s.size(); i++) {
        if(s[i] == '\\') {
            s.erase(i, 1);
            switch(s[i]) {
				case 'n':
					s[i] = '\n'; break;
				case 't':
					s[i] = '\t'; break;
				case 'a':
					s[i] = '\a'; break;
				case 'f':
					s[i] = '\f'; break;
				case 'r':
					s[i] = '\r'; break;
				case 'b':
					s[i] = '\b'; break;
				case 'v':
					s[i] = '\v'; break;
				case '0':
					s[i] = '\0'; break;
				case '\n': //for multiline string
					s.erase(i, 1); break;
			}
        }
    }
    return s;
}

%}

KEYWORD (if|else|for|while|do|break|int|char|float|double|void|return|switch|case|default|continue)
WHITESPACE [ \t\f\r\v]+ 
LETTER [a-zA-Z]
DIGIT [0-9]
ESCAPE_SEQUENCE \\[nt\\'afrbv0]
CHARACTER [ -~]{-}[\\'"]
MULTI_LINE_SEQUENCE \\\n

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

'({ESCAPE_SEQUENCE}|{CHARACTER})' {
	string temp = yytext;
	temp.erase(0,1);
	temp.pop_back();
	
	temp = replaceWithEscapeSeq(temp);
	
	string token = string("<CONST_CHAR, ") + temp + "> ";
	tokenout <<  token;
	logout << "Line no " << line_count << " TOKEN <CONST_CHAR> Lexeme " << yytext << " found --> " << token << "\n";
	st.insert(yytext, "CONST_CHAR"); 
	st.printAllScopeTables(logout);
}

\"({ESCAPE_SEQUENCE}|{CHARACTER}|{MULTI_LINE_SEQUENCE})*\" {
	string temp = yytext;
	temp.erase(0,1);
	temp.pop_back();
	
	temp = replaceWithEscapeSeq(temp);
	
	string token = string("<STRING, ") + temp + "> ";
	tokenout <<  token;
	logout << "Line no " << line_count << " TOKEN <STRING> Lexeme " << yytext << " found --> " << token << "\n";
}

(\/\/(.|{MULTI_LINE_SEQUENCE})*)|(\/\*(.|[\n])*\*\/) {//    /**/*/ EITA BAAKI  (\/\/.*)|
	logout << "Line no " << line_count << " TOKEN <COMMENT> Lexeme " << yytext << " found\n";
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

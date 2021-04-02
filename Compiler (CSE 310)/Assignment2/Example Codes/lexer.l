%option noyywrap

%x COMMENT_STATE

%{
#include<bits/stdc++.h>
#include "SymbolTable.cpp"
using namespace std;

ofstream logout;
ofstream tokenout;

SymbolTable st(7);
int line_count;
int error_count;
bool hasCommentEnded;
bool hasWrongEscapeSeq;
string comment;

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
				case '\"':
				case '\'':
				case '\\':
					break;
				case '\n': //for multiline string
					s.erase(i, 1); break;
				case '\r': //for multiline string
					if(s[i+1] == '\n') s.erase(i, 2);
					else hasWrongEscapeSeq = 1;
					break;
				default:
					hasWrongEscapeSeq = 1;
			}
        }
    }
    return s;
}

int countNewLines(string s) { //newlines that occured inside multi-line string, comment etc
	int count = 0;
	for(int i=0; i<s.size(); i++) {
		if(s[i] == '\n') count ++;
	}
	return count;
}

void printKeywordToken(string tokenName) {
	tokenout << "<" + tokenName << "> ";
}

string printToken(string tokenName, string tokenAttribute = yytext) {
	string token =  string("<") + tokenName + ", " + tokenAttribute + "> ";
	tokenout << token;
	return token;
}

void printLog(string tokenName, string token = "") {
	logout << "Line no " << line_count << ": Token <" + tokenName + "> Lexeme " << yytext << " found";
	if(token != "")
		logout << " --> " << token;
	logout << "\n\n";
}

void printTokenAndLog(string tokenName) {
	printToken(tokenName);
	printLog(tokenName);
}

void handleError(string message, string lexeme = yytext) {
	logout << "Error at line no " << line_count << ": " << message << " " << lexeme << "\n\n";
	error_count ++;
}

void handleScopeTable(string tokenName) {
	if(st.insert(yytext, tokenName, logout))
		st.printAllScopeTables(logout);
}
%}

NEWLINE (\n|\r\n)
KEYWORD (if|else|for|while|do|break|int|char|float|double|void|return|switch|case|default|continue)
WHITESPACE [ \t\f\r\v]+ 
LETTER [a-zA-Z]
DIGIT [0-9]
ESCAPE_SEQUENCE \\[nt\\'\"afrbv0]
CHARACTER [^\\'\"]
%%

{WHITESPACE} {}

{NEWLINE} {line_count ++;}

{KEYWORD} {
	string tokenName = yytext;
	transform(tokenName.begin(), tokenName.end(), tokenName.begin(), ::toupper);
	printKeywordToken(tokenName);
	printLog(tokenName);
}

[\+\-] {printTokenAndLog("ADDOP");}
[\*\/%] {printTokenAndLog("MULOP");}
("++"|"--") {printTokenAndLog("INCOP");}
("<"|"<="|">"|">="|"=="|"!=") {printTokenAndLog("RELOP");}
"=" {printTokenAndLog("ASSIGNOP");}
("&&"|"||") {printTokenAndLog("LOGICOP");}
"!" {printTokenAndLog("NOT");}
"(" {printTokenAndLog("LPAREN");}
")" {printTokenAndLog("RPAREN");}
"{" {printTokenAndLog("LCURL"); st.enterScope();}
"}" {printTokenAndLog("RCURL"); st.exitScope();}
"[" {printTokenAndLog("LTHIRD");}
"]" {printTokenAndLog("RTHIRD");}
"," {printTokenAndLog("COMMA");}
";" {printTokenAndLog("SEMICOLON");}
	
({LETTER}|_)+({LETTER}|{DIGIT}|_)* { //identifier
	printTokenAndLog("ID");
	handleScopeTable("ID");
}

{DIGIT}+ { //integer
	printTokenAndLog("CONST_INT");
	handleScopeTable("CONST_INT");
}	

{DIGIT}*\.{DIGIT}+(E[\+\-]?{DIGIT}+)? { //floating point
	printTokenAndLog("CONST_FLOAT");
	handleScopeTable("CONST_FLOAT");
}

'({ESCAPE_SEQUENCE}|{CHARACTER})' { //character
	string temp = yytext;
	temp.erase(0,1);
	temp.pop_back();
	
	temp = replaceWithEscapeSeq(temp);
	
	string token = printToken("CONST_CHAR", temp);
	printLog("CONST_CHAR", token);
	handleScopeTable("CONST_CHAR");
}

\"((\\.)|{CHARACTER}|(\\{NEWLINE}))*\" { //string
	string temp = yytext;
	temp.erase(0,1);
	temp.pop_back();
	
	temp = replaceWithEscapeSeq(temp);
	
	if(hasWrongEscapeSeq) {
		handleError("Wrong Escape Sequence");
	}
	else {
		string token = printToken("STRING", temp);
		printLog("STRING", token);
	}
	hasWrongEscapeSeq = 0;
	line_count += countNewLines(yytext);
}

(\/\/(.|(\\{NEWLINE}))*) { //single-line comment
	printLog("COMMENT");
	line_count += countNewLines(yytext);
}

\/\* { //multi-line comment
	hasCommentEnded = 0;
	comment = yytext;
	yymore();
	BEGIN COMMENT_STATE;
}

({DIGIT}|\.)+ {handleError("Too many decimal points");}

({DIGIT}|[\.E])+ {handleError("Ill formed number");}

({DIGIT}|{LETTER}|_)+ {handleError("Invalid prefix on ID or invalid suffix on Number");}

'' {handleError("Empty character constant error");}

('[^'\r\n]*)|('[\\\"]') {handleError("Unterminated character");}

'[^'\r\n]+' {handleError("Multi character constant error");}

\"((\\.)|{CHARACTER}|(\\{NEWLINE}))* {
	handleError("Unterminated String");
	line_count += countNewLines(yytext);
}

. {handleError("Unrecognized character");}

<COMMENT_STATE>.|\n {comment = yytext; yymore();}

<COMMENT_STATE>\*\/ {
	hasCommentEnded = 1;
	printLog("COMMENT");
	line_count += countNewLines(yytext);
	BEGIN INITIAL;
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
	error_count = 0;
	hasCommentEnded = 1;
	hasWrongEscapeSeq = 0;
	
	yylex();
	
	if(!hasCommentEnded) {
		handleError("Unterminated Comment", comment);
		line_count += countNewLines(comment);
	}
	st.printAllScopeTables(logout);
	logout << "Total lines: " << line_count << "\nTotal errors: " << error_count << "\n";
	
	fclose(yyin);
	tokenout.close();
	logout.close();

	return 0;
}

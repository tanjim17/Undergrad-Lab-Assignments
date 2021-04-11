%option noyywrap

%x COMMENT_STATE

%{
#include<bits/stdc++.h>
#include "SymbolTable.cpp"
#include "y.tab.h"
//#define YYSTYPE SymbolInfo*

using namespace std;

ofstream logout;
ofstream tokenout;

SymbolTable st(7);
int line_count;
int error_count;
bool hasCommentEnded;
bool hasWrongEscapeSeq;
string comment;
map<string, int> keyword_map = {{"if", IF}, {"else", ELSE}, {"for", FOR}, {"while", WHILE}, {"int", INT}, {"float", FLOAT}, {"void", VOID}, {"return", RETURN}};
//extern YYSTYPE yylval;

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
					s[i] = ' '; break;
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

/*void printLog(string tokenName, string token = "") {
	logout << "Line no " << line_count << ": Token <" + tokenName + "> Lexeme " << yytext << " found";
	if(token != "")
		logout << " --> " << token;
	logout << "\n\n";
}*/

void printTokenAndLog(string tokenName) {
	printToken(tokenName);
	//printLog(tokenName);
}

void handleError(string message, string lexeme = yytext) {
	logout << "Error at line no " << line_count << ": " << message << " " << lexeme << "\n\n";
	error_count ++;
}

void handleScopeTable(string tokenName) {
}
%}

NEWLINE (\n|\r\n)
KEYWORD (if|else|for|while|do|break|int|char|float|double|void|return|switch|case|default|continue)
WHITESPACE [ \t\f\r\v]+ 
LETTER [a-zA-Z]
DIGIT [0-9]
ESCAPE_SEQUENCE \\[nt\\'\"afrbv0]
CHARACTER [^\\'\"\n]
FLOAT ({DIGIT}+|({DIGIT}*\.{DIGIT}*))(E[\+\-]?{DIGIT}+)?

%%

{WHITESPACE} {}

{NEWLINE} {line_count ++;}

{KEYWORD} {
	string tokenName = yytext;
	transform(tokenName.begin(), tokenName.end(), tokenName.begin(), ::toupper);
	printKeywordToken(tokenName);
	//printLog(tokenName);
	yylval.x = new SymbolInfo(yytext, tokenName);
	return keyword_map[yytext];
}

[\+\-] {
	printTokenAndLog("ADDOP");
	yylval.x = new SymbolInfo(yytext, "ADDOP");
	return ADDOP;
}
[\*\/%] {
	printTokenAndLog("MULOP");
	yylval.x = new SymbolInfo(yytext, "MULOP");
	return MULOP;
}
"++" {
	printTokenAndLog("INCOP");
	yylval.x = new SymbolInfo(yytext, "INCOP");
	return INCOP;
}
"--" {
	printTokenAndLog("DECOP");
	yylval.x = new SymbolInfo(yytext, "DECOP");
	return DECOP;
}
("<"|"<="|">"|">="|"=="|"!=") {
	printTokenAndLog("RELOP");
	yylval.x = new SymbolInfo(yytext, "RELOP");
	return RELOP;
}
"=" {
	printTokenAndLog("ASSIGNOP");
	yylval.x = new SymbolInfo(yytext, "ASSIGNOP");
	return ASSIGNOP;	
}
("&&"|"||") {
	printTokenAndLog("LOGICOP");
	yylval.x = new SymbolInfo(yytext, "LOGICOP");
	return LOGICOP;
}
"!" {
	printTokenAndLog("NOT");
	yylval.x = new SymbolInfo(yytext, "NOT");
	return NOT;
}
"(" {
	printTokenAndLog("LPAREN");
	yylval.x = new SymbolInfo(yytext, "LPAREN");
	return LPAREN;
}
")" {
	printTokenAndLog("RPAREN");
	yylval.x = new SymbolInfo(yytext, "RPAREN");
	return RPAREN;
}
"{" {
	st.enterScope();
	printTokenAndLog("LCURL");
	yylval.x = new SymbolInfo(yytext, "LCURL");
	return LCURL;	
}
"}" {
	//st.exitScope();
	printTokenAndLog("RCURL");
	yylval.x = new SymbolInfo(yytext, "RCURL");
	return RCURL;
}
"[" {
	printTokenAndLog("LTHIRD");
	yylval.x = new SymbolInfo(yytext, "LTHIRD");
	return LTHIRD;	
}
"]" {
	printTokenAndLog("RTHIRD");
	yylval.x = new SymbolInfo(yytext, "RTHIRD");
	return RTHIRD;
}
"," {
	printTokenAndLog("COMMA");
	yylval.x = new SymbolInfo(yytext, "COMMA");
	return COMMA;
}
";" {
	printTokenAndLog("SEMICOLON");
	yylval.x = new SymbolInfo(yytext, "SEMICOLON");
	return SEMICOLON;
}
	
({LETTER}|_)+({LETTER}|{DIGIT}|_)* { //identifier
	printTokenAndLog("ID");
	handleScopeTable("ID");
	yylval.x = new SymbolInfo(yytext, "ID");
	return ID;
}

{DIGIT}+ { //integer
	printTokenAndLog("CONST_INT");
	handleScopeTable("CONST_INT");
	yylval.x = new SymbolInfo(yytext, "CONST_INT");
	return CONST_INT;
}	

{FLOAT} { //floating point
	printTokenAndLog("CONST_FLOAT");
	handleScopeTable("CONST_FLOAT");
	yylval.x = new SymbolInfo(yytext, "CONST_FLOAT");
	return CONST_FLOAT;
}

'({ESCAPE_SEQUENCE}|{CHARACTER}|\")' { //character
	string temp = yytext;
	temp.erase(0,1);
	temp.pop_back();
	
	temp = replaceWithEscapeSeq(temp);
	
	string token = printToken("CONST_CHAR", temp);
	//printLog("CONST_CHAR", token);
	handleScopeTable("CONST_CHAR");
}

\"((\\.)|{CHARACTER}|'|(\\{NEWLINE}))*\" { //string
	string temp = yytext;
	temp.erase(0,1);
	temp.pop_back();
	
	hasWrongEscapeSeq = 0;
	temp = replaceWithEscapeSeq(temp);
	
	if(hasWrongEscapeSeq) {
		handleError("Wrong Escape Sequence");
	}
	else {
		string token = printToken("STRING", temp);
		//printLog("STRING", token);
	}
	line_count += countNewLines(yytext);
}

(\/\/(.|(\\{NEWLINE}))*) { //single-line comment
	//printLog("COMMENT");
	line_count += countNewLines(yytext);
}

\/\* { //multi-line comment
	hasCommentEnded = 0;
	comment = yytext;
	yymore();
	BEGIN COMMENT_STATE;
}

({DIGIT}|\.)+ {handleError("Too many decimal points");}

({DIGIT}|{LETTER}|_)+|{FLOAT}({LETTER}|_)+ {handleError("Invalid prefix on ID or invalid suffix on Number");}

({DIGIT}|\.)*E[\+\-]?({DIGIT}|\.)* {handleError("Ill formed number");}

'' {handleError("Empty character constant error");}

'[^'\r\n]*|'\\' {handleError("Unterminated character");}

'[^'\r\n]+' {handleError("Multi character constant error");}

\"([^\"\r\n]*|(\\{NEWLINE}))* {
	handleError("Unterminated String");
	line_count += countNewLines(yytext);
}

. {handleError("Unrecognized character");}

<COMMENT_STATE>.|\n {comment = yytext; yymore();}

<COMMENT_STATE>\*\/ {
	hasCommentEnded = 1;
	//printLog("COMMENT");
	line_count += countNewLines(yytext);
	BEGIN INITIAL;
}

%%

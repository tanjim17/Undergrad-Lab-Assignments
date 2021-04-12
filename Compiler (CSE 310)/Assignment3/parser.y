%{
#include <bits/stdc++.h>
#include "SymbolTable.cpp"
//#define YYSTYPE SymbolInfo*

using namespace std;

int yyparse(void);
int yylex(void);

FILE* fp;
ofstream log_;
ofstream error;
bool hasScopeStarted;
vector<pair<string, bool>> declaration_list; // bool value  determines if its an array

extern FILE* yyin;
extern int line_count;
extern int error_count;
extern SymbolTable st;


void yyerror(char *s)
{
	//write your code
}

void printLog(string grammar, string sentence) {
	log_ << "At line no: " << line_count << " " << grammar << "\n\n" << sentence << "\n\n";
}

void printError(string message) {
	error << "Error At Line " << line_count << ": " << message << "\n\n";
}

vector<string> getParamTypes(string s) {
	replace(s.begin(), s.end(), ',', ' ');
	istringstream ss(s);
	vector<string> paramTypes;
    string token;
    while (ss >> token) {
    	if(token == "void") { //int f(void) ??
    		printError("Void data-type for params");
    		break;
    	}
    	if(token == "int" || token == "float") {
			paramTypes.push_back(token);
    	}
    }
    return paramTypes;
}

void insertParamsInSymbolTable(string s) {
	replace(s.begin(), s.end(), ',', ' ');
	
	//tokenize
	istringstream ss(s);
    string token;
    string currentDataType;
    while (ss >> token) {
    	if(token == "void") {
    		printError("Void data-type for params");
    		break;
    	}  	
    	if(token == "int" || token == "float") {
			currentDataType = token;
    	} else {
    		st.insert(new SymbolInfo(token, "ID", currentDataType));
    	}
    }
}

void insertVarsInSymbolTable(string dataType) {
	if(dataType == "void") {
   		printError("Void data-type for variables");
   		return;
    }
	string arrayDataType = dataType + "_array";
	string dtype;
	for(pair<string, bool> x : declaration_list) {
		if(x.second) dtype = arrayDataType;
		else dtype = dataType;
		if(!st.insert(new SymbolInfo(x.first, "ID", dtype))) {
			printError("Multiple Declaration of " + x.first);
		}
	}
	declaration_list.clear();
}

bool verifyWithDeclaration(string returnType, string name, vector<string> paramTypes = {}) {
	SymbolInfo* funcInfo = st.lookUp(name);
	if(funcInfo != NULL) {
		if(returnType != funcInfo->getDataType()) return 0;
		
		vector<string> declaredParamTypes = funcInfo->getParamTypes();
		if(paramTypes.size() != declaredParamTypes.size()) return 0;
		
		for(int i=0; i<paramTypes.size(); i++) 
			if(paramTypes[i] != declaredParamTypes[i]) return 0;
			
		return 1;
		
	} else {
		return 1;
	}
}


%}

%union {SymbolInfo* x;}
%token <x> IF ELSE FOR WHILE RETURN
%token <x> INT FLOAT VOID
%token <x> ADDOP MULOP INCOP DECOP RELOP ASSIGNOP LOGICOP NOT
%token <x> LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD COMMA SEMICOLON
%token <x> ID PRINTLN CONST_INT CONST_FLOAT 
%type <x> start program unit func_declaration func_definition parameter_list compound_statement var_declaration type_specifier declaration_list statements statement expression_statement variable expression logic_expression rel_expression simple_expression term unary_expression factor argument_list arguments

%%

start : program {
	st.printAllScopeTables(log_);
}
	;

program : program unit {
	$$ = new SymbolInfo($1->getName() + "\n" + $2->getName(), "program");
	printLog("program : program unit", $$->getName());
}
	| unit {
	$$ = new SymbolInfo($1->getName(), "program");
	printLog("program : unit", $$->getName());
}
	;
	
unit : var_declaration {printLog("unit : var_declaration", $1->getName());}
     | func_declaration {printLog("unit : func_declaration", $1->getName());}
     | func_definition {printLog("unit : func_definition", $1->getName());}
     ;
     
func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON {
	st.insert( new SymbolInfo($2->getName(), "ID", $1->getName(), getParamTypes($4->getName())) );
	$$ = new SymbolInfo($1->getName() + " " + $2->getName() + "(" + $4->getName() + ");", "func_declaration");
	printLog("func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON", $$->getName());
}
		| type_specifier ID LPAREN RPAREN SEMICOLON {
	st.insert( new SymbolInfo($2->getName(), "ID", $1->getName()) );
	$$ = new SymbolInfo($1->getName() + " " + $2->getName() + "();", "func_declaration");
	printLog("func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON", $$->getName());
}
		;
		 
func_definition : type_specifier ID LPAREN parameter_list RPAREN {
	vector<string> paramTypes = getParamTypes($4->getName());
	
	SymbolInfo* funcInfo = st.lookUp($2->getName());
	if(funcInfo == NULL) {
		st.insert( new SymbolInfo($2->getName(), "ID", $1->getName(), paramTypes) );
	} else {
		if(!verifyWithDeclaration($1->getName(), $2->getName(), paramTypes)) {
			printError("Function Declaration MisMatch");
		} 
	}
	
	st.enterScope();
	insertParamsInSymbolTable($4->getName());
	hasScopeStarted = 1;
} compound_statement {
	$$ = new SymbolInfo($1->getName() + " " + $2->getName() + "(" + $4->getName() + ")" + $7->getName(), "func_definition");
	st.exitScope();
	printLog("func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement", $$->getName());
}
		| type_specifier ID LPAREN RPAREN {	
	SymbolInfo* funcInfo = st.lookUp($2->getName());
	if(funcInfo != NULL) {
		st.insert( new SymbolInfo($2->getName(), "ID", $1->getName()) );
	} else {
		if(!verifyWithDeclaration($1->getName(), $2->getName())) {
			printError("Function Declaration MisMatch");
		} 
	}
	
	st.enterScope();
	hasScopeStarted = 1;	
} compound_statement {
	$$ = new SymbolInfo($1->getName() + " " + $2->getName() + "()" + $6->getName(), "func_definition");
	st.exitScope();
	printLog("func_definition : type_specifier ID LPAREN RPAREN compound_statement", $$->getName());
}
 		;				


parameter_list : parameter_list COMMA type_specifier ID {
	$$ = new SymbolInfo($1->getName() + "," + $3->getName() + " " + $4->getName(), "parameter_list");
	printLog("parameter_list : parameter_list COMMA type_specifier ID", $$->getName());
}
		| parameter_list COMMA type_specifier { //this is valid for func declaration, not definition. gotta solve?
	$$ = new SymbolInfo($1->getName() + "," + $3->getName(), "parameter_list");
	printLog("parameter_list : parameter_list COMMA type_specifier", $$->getName());
}
 		| type_specifier ID {
	$$ = new SymbolInfo($1->getName() + " " + $2->getName(), "parameter_list");
	printLog("parameter_list : type_specifier ID", $$->getName());
}
		| type_specifier {printLog("parameter_list : type_specifier", $1->getName());}
 		;

 		
compound_statement : LCURL {
	if(hasScopeStarted) hasScopeStarted = 0;
	else st.enterScope();
} statements RCURL {
	$$ = new SymbolInfo("{\n" + $3->getName() + "\n}", "compound_statement");
	printLog("compound_statement : LCURL statements RCURL", $$->getName());
}
 		    | LCURL {
	if(hasScopeStarted) hasScopeStarted = 0;
	else st.enterScope();
} RCURL {
 	$$ = new SymbolInfo("{}", "compound_statement");
	printLog("compound_statement : LCURL RCURL", $$->getName());
}
 		    ;
 		    
var_declaration : type_specifier declaration_list SEMICOLON {
	insertVarsInSymbolTable($1->getName());
	$$ = new SymbolInfo($1->getName() + " " + $2->getName() + ";", "var_declaration");
	printLog("var_declaration : type_specifier declaration_list SEMICOLON", $$->getName());
}
 		 ;
 		 
type_specifier : INT {printLog("type_specifier : INT", $1->getName());}
 		| FLOAT {printLog("type_specifier : FLOAT", $1->getName());}
 		| VOID {printLog("type_specifier : VOID", $1->getName());}
 		;
 		
declaration_list : declaration_list COMMA ID {
	declaration_list.push_back({$3->getName(), 0});
	$$ = new SymbolInfo($1->getName() + "," + $3->getName(), "declaration_list");
	printLog("declaration_list : declaration_list COMMA ID", $$->getName());
}
 		  | declaration_list COMMA ID LTHIRD CONST_INT RTHIRD {
 	declaration_list.push_back({$3->getName(), 1});
	$$ = new SymbolInfo($1->getName() + "," + $3->getName() + "[" + $5->getName() + "]", "declaration_list");
	printLog("declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD", $$->getName());
}
 		  | ID {
 	declaration_list.push_back({$1->getName(), 0});
 	printLog("declaration_list : ID", $1->getName());
 }
 		  | ID LTHIRD CONST_INT RTHIRD {
 	declaration_list.push_back({$1->getName(), 1});
	$$ = new SymbolInfo($1->getName() + "[" + $3->getName() + "]", "declaration_list");
	printLog("declaration_list : ID LTHIRD CONST_INT RTHIRD", $$->getName());
}
 		  ;
 		  
statements : statement {printLog("statements : statement", $1->getName());}
	   | statements statement {
	$$ = new SymbolInfo($1->getName() + "\n" + $2->getName(), "statements");
	printLog("statements : statements statement", $$->getName());
}
	   ;
	   
statement : var_declaration {printLog("statement : var_declaration", $1->getName());}
	  | expression_statement {printLog("statement : expression_statement", $1->getName());}
	  | compound_statement {printLog("statement : compound_statement", $1->getName()); st.exitScope();}
	  | FOR LPAREN expression_statement expression_statement expression RPAREN statement {
	$$ = new SymbolInfo("(" + $2->getName() + $3->getName() + $4->getName() + ")" + $6->getName(), "statement");
	printLog("statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement", $$->getName());
}
	  | IF LPAREN expression RPAREN statement {
	$$ = new SymbolInfo("if (" +  $3->getName() + ")" + $5->getName(), "statement");
	printLog("statement : IF LPAREN expression RPAREN statement", $$->getName());
}
	  | IF LPAREN expression RPAREN statement ELSE statement {
	$$ = new SymbolInfo("if (" +  $3->getName() + ")" + $5->getName() + " else " + $7->getName(), "statement");
	printLog("statement : IF LPAREN expression RPAREN statement ELSE statement", $$->getName());
}
	  | WHILE LPAREN expression RPAREN statement {
	$$ = new SymbolInfo("while (" +  $3->getName() + ")" + $5->getName(), "statement");
	printLog("statement : WHILE LPAREN expression RPAREN statement", $$->getName());
}
	  | PRINTLN LPAREN ID RPAREN SEMICOLON {
	$$ = new SymbolInfo("println(" +  $3->getName() + ");", "statement");
	printLog("statement : PRINTLN LPAREN ID RPAREN SEMICOLON", $$->getName());
}
	  | RETURN expression SEMICOLON {
	$$ = new SymbolInfo("return " +  $2->getName() + ";", "statement");
	printLog("statement : RETURN expression SEMICOLON", $$->getName());
}
	  ;
	  
expression_statement : SEMICOLON {printLog("expression_statement : SEMICOLON", $1->getName());}
			| expression SEMICOLON {
	$$ = new SymbolInfo($1->getName() + ";", "expression_statement");
	printLog("expression_statement : expression SEMICOLON", $$->getName());
}
			;
	  
variable : ID {printLog("variable : ID", $1->getName());}
	 | ID LTHIRD expression RTHIRD {
	$$ = new SymbolInfo($1->getName() + "[" + $3->getName() + "]", "variable");
	printLog("variable : ID LTHIRD expression RTHIRD", $$->getName());
}
	 ;
	 
 expression : logic_expression {printLog("expression : logic_expression", $1->getName());}
	   | variable ASSIGNOP logic_expression {
	$$ = new SymbolInfo($1->getName() + "=" + $3->getName(), "expression");
	printLog("expression : variable ASSIGNOP logic_expression", $$->getName());
}
	   ;

logic_expression : rel_expression {printLog("logic_expression : rel_expression", $1->getName());}
		 | rel_expression LOGICOP rel_expression {
	$$ = new SymbolInfo($1->getName() + $2->getName() + $3->getName(), "logic_expression");
	printLog("logic_expression : rel_expression LOGICOP rel_expression", $$->getName());
}
		 ;

rel_expression : simple_expression {printLog("rel_expression : simple_expression", $1->getName());}
		| simple_expression RELOP simple_expression {
	$$ = new SymbolInfo($1->getName() + $2->getName() + $3->getName(), "rel_expression");
	printLog("rel_expression : simple_expression RELOP simple_expression", $$->getName());
}
		;

simple_expression : term {printLog("simple_expression : term", $1->getName());}
		  | simple_expression ADDOP term {
	$$ = new SymbolInfo($1->getName() + $2->getName() + $3->getName(), "simple_expression");
	printLog("simple_expression : simple_expression ADDOP term", $$->getName());
}
		  ;

term :	unary_expression {printLog("term : unary_expression", $1->getName());}
     |  term MULOP unary_expression {
	$$ = new SymbolInfo($1->getName() + $2->getName() + $3->getName(), "term");
	printLog("term : term MULOP unary_expression", $$->getName());
}
     ;

unary_expression : ADDOP unary_expression {
	$$ = new SymbolInfo($1->getName() + $2->getName(), "unary_expression");
	printLog("unary_expression : ADDOP unary_expression", $$->getName());
}
		 | NOT unary_expression {
	$$ = new SymbolInfo("!" + $2->getName(), "unary_expression");
	printLog("unary_expression : NOT unary_expression", $$->getName());
}
		 | factor {printLog("unary_expression : factor", $1->getName());}
		 ;
	
factor : variable {printLog("factor : variable", $1->getName());}
	| ID LPAREN argument_list RPAREN {
	$$ = new SymbolInfo($1->getName() + "(" + $3->getName() + ")", "factor");
	printLog("factor : ID LPAREN argument_list RPAREN", $$->getName());
}
	| LPAREN expression RPAREN {
	$$ = new SymbolInfo("(" + $2->getName() + ")", "factor");
	printLog("factor : LPAREN expression RPAREN", $$->getName());
}
	| CONST_INT {printLog("factor : CONST_INT", $1->getName());}
	| CONST_FLOAT {printLog("factor : CONST_FLOAT", $1->getName());}
	| variable INCOP {
	$$ = new SymbolInfo($1->getName() + "++", "factor");
	printLog("factor : variable INCOP", $$->getName());
}
	| variable DECOP {
	$$ = new SymbolInfo($1->getName() + "--", "factor");
	printLog("factor : variable DECOP", $$->getName());
}
	;
	
argument_list : arguments {printLog("argument_list : arguments", $1->getName());}
			  | { $$ = new SymbolInfo("", "argument_list");
		 		printLog("argument_list : ", $$->getName());
			  }
			  ;
	
arguments : arguments COMMA logic_expression {
	$$ = new SymbolInfo($1->getName() + ", " + $3->getName(), "arguments");
	printLog("arguments : arguments COMMA logic_expression", $$->getName());
}
	      | logic_expression {printLog("arguments : logic_expression", $1->getName());}
	      ;
 

%%
int main(int argc,char *argv[])
{
	// PRINTLN, DECOP ei token gula baki
	if((fp=fopen(argv[1],"r"))==NULL)
	{
		printf("Cannot Open Input File.\n");
		exit(1);
	}

	/*fp2= fopen(argv[2],"w");
	fclose(fp2);
	fp3= fopen(argv[3],"w");
	fclose(fp3);
	
	fp2= fopen(argv[2],"a");
	fp3= fopen(argv[3],"a");*/
	log_.open("log.txt");
	error.open("error.txt");
	
	line_count = 1;
	error_count = 0;
	hasScopeStarted = 0;
	yyin=fp;
	yyparse();
	
	fseek(yyin, -1, SEEK_END);
	if(fgetc(yyin) == '\n') line_count --;
	log_ << "Total Lines: " << line_count << "\n";
	log_ << "Total Errors: " << error_count;
	//fclose(fp2);
	//fclose(fp3);
	fclose(yyin);
	log_.close();
	error.close();
	
	return 0;
}


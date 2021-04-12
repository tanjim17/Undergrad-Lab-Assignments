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
vector<string> argumentTypes;
vector<string> undefinedFunctions;

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
    		if( !st.insert(new SymbolInfo(token, "ID", currentDataType)) ) {
    			printError("Multiple Declaration of " + token);
    		}
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

void checkAndInsertFunc(string returnType, string name, vector<string> paramTypes = {}) {
	SymbolInfo* funcInfo = st.lookUp(name);
	if(funcInfo == NULL) {
		st.insert( new SymbolInfo(name, "ID", returnType, 1, paramTypes) );
	} else {
		if (find(undefinedFunctions.begin(), undefinedFunctions.end(), name) != undefinedFunctions.end()) {
			undefinedFunctions.erase(remove(undefinedFunctions.begin(), undefinedFunctions.end(), name), undefinedFunctions.end());
			if(!verifyWithDeclaration(returnType, name, paramTypes)) {
				printError("Function Declaration MisMatch");
			} 
		} else {
			printError("Multiple Definition of " + name);
		}		
	}
}

void verifyFunctionCall(SymbolInfo* funcInfo) {
	if(funcInfo->getDataType() == "void") {
		printError("Void function in expression"); return;
	}
	vector<string> declaredParamTypes = funcInfo->getParamTypes();
	if(argumentTypes.size() != declaredParamTypes.size()) {
		printError("Argument number mismatch"); return;
	}
	for(int i=0; i<argumentTypes.size(); i++) {
		if(argumentTypes[i] != declaredParamTypes[i]) {
			printError("Argument type mismatch"); return;
		}
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
	string name = $2->getName();
	if(!st.insert( new SymbolInfo(name, "ID", $1->getName(), 1, getParamTypes($4->getName())) )) {
		printError("Multiple declaration of " + name);
	};
	$$ = new SymbolInfo($1->getName() + " " + name + "(" + $4->getName() + ");", "func_declaration");
	undefinedFunctions.push_back(name);
	printLog("func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON", $$->getName());
}
		| type_specifier ID LPAREN RPAREN SEMICOLON {
	string name = $2->getName();
	if(!st.insert( new SymbolInfo(name, "ID", $1->getName(), 1) )) {
		printError("Multiple declaration of " + name);
	};
	$$ = new SymbolInfo($1->getName() + " " + name + "();", "func_declaration");
	undefinedFunctions.push_back(name);
	printLog("func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON", $$->getName());
}
		;
		 
func_definition : type_specifier ID LPAREN parameter_list RPAREN {
	vector<string> paramTypes = getParamTypes($4->getName());
	checkAndInsertFunc($1->getName(), $2->getName(), paramTypes);
	st.enterScope();
	insertParamsInSymbolTable($4->getName());
	hasScopeStarted = 1;
} compound_statement {
	$$ = new SymbolInfo($1->getName() + " " + $2->getName() + "(" + $4->getName() + ")" + $7->getName(), "func_definition");
	undefinedFunctions.clear();
	st.exitScope();
	printLog("func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement", $$->getName());
}
		| type_specifier ID LPAREN RPAREN {	
	checkAndInsertFunc($1->getName(), $2->getName());
	st.enterScope();
	hasScopeStarted = 1;	
} compound_statement {
	$$ = new SymbolInfo($1->getName() + " " + $2->getName() + "()" + $6->getName(), "func_definition");
	undefinedFunctions.clear();
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
	  
variable : ID {
	string name = $1->getName();
	SymbolInfo* temp = st.lookUp(name);
	if(temp == NULL) {
		printError("Undeclared Variable: " + name);
	} else {
		if(temp->getIsFunction())
			printError(name + " is function, not variable"); // gotta write it in more places
		string dataType = temp->getDataType();
		if (dataType.find("array") != string::npos) //symbolinfo banaite hobe
			 printError("Array without index");
		else {
			$$ = new SymbolInfo(name, "variable", dataType);
			printLog("variable : ID", name);
		}
	}
}
	 | ID LTHIRD expression RTHIRD {
	if($3->getDataType() != "int") {
		printError("Non-integer Array Index");
	} else {
		string name = $1->getName();
		SymbolInfo* temp = st.lookUp(name);
		if(temp == NULL) {
			printError("Undeclared Variable: " + name);
		} else {
			string dataType = temp->getDataType();
			if (dataType.find("array") != string::npos) { // todo: float index
				$$ = new SymbolInfo(name + "[" + $3->getName() + "]", "variable", dataType.substr(0, dataType.find("_")));
				printLog("variable : ID LTHIRD expression RTHIRD", name);
			} else {
				printError("Index without array");
			}
		}	
	}
}
	 ;
	 
expression : logic_expression {printLog("expression : logic_expression", $1->getName());}
	   | variable ASSIGNOP logic_expression {
	if($1->getDataType() == $3->getDataType()) {
		$$ = new SymbolInfo($1->getName() + "=" + $3->getName(), "expression");
		printLog("expression : variable ASSIGNOP logic_expression", $$->getName());	
	} else {
		printError("Type Mismatch");
	}
}
	   ;

logic_expression : rel_expression {printLog("logic_expression : rel_expression", $1->getName());}
		 | rel_expression LOGICOP rel_expression {
	$$ = new SymbolInfo($1->getName() + $2->getName() + $3->getName(), "logic_expression", "int");
	printLog("logic_expression : rel_expression LOGICOP rel_expression", $$->getName());
}
		 ;

rel_expression : simple_expression {printLog("rel_expression : simple_expression", $1->getName());}
		| simple_expression RELOP simple_expression {
	$$ = new SymbolInfo($1->getName() + $2->getName() + $3->getName(), "rel_expression", "int");
	printLog("rel_expression : simple_expression RELOP simple_expression", $$->getName());
}
		;

simple_expression : term {printLog("simple_expression : term", $1->getName());}
		  | simple_expression ADDOP term {
	string type = "int";
    if($1->getDataType() == "float" || $2->getDataType() != "float") type = "float";
	$$ = new SymbolInfo($1->getName() + $2->getName() + $3->getName(), "simple_expression", type);
	printLog("simple_expression : simple_expression ADDOP term", $$->getName());
}
		  ;

term :	unary_expression {printLog("term : unary_expression", $1->getName());}
     |  term MULOP unary_expression {
    string type1 = $1->getDataType();
    string op = $2->getName();
    string type2 = $3->getDataType(); 
    if(op == "%" && (type1 != "int" || type2 != "int")) {
    	printError("Integer operand on modulus operator");
    } else {
    	string type = "int";
    	if(type1 == "float" || type2 == "float") type = "float";
		$$ = new SymbolInfo($1->getName() + $2->getName() + $3->getName(), "term", type);
		printLog("term : term MULOP unary_expression", $$->getName());
    }
}
     ;

unary_expression : ADDOP unary_expression {
	$$ = new SymbolInfo($1->getName() + $2->getName(), "unary_expression", $2->getDataType());
	printLog("unary_expression : ADDOP unary_expression", $$->getName());
}
		 | NOT unary_expression {
	$$ = new SymbolInfo("!" + $2->getName(), "unary_expression", $2->getDataType());
	printLog("unary_expression : NOT unary_expression", $$->getName());
}
		 | factor {printLog("unary_expression : factor", $1->getName());}
		 ;
	
factor : variable {printLog("factor : variable", $1->getName());}
	| ID LPAREN argument_list RPAREN {
	string name = $1->getName();
	SymbolInfo* temp = st.lookUp(name);
	if(temp == NULL) {
		printError("Undeclared Function: " + name);
	} else {
		verifyFunctionCall(temp);
		$$ = new SymbolInfo(name + "(" + $3->getName() + ")", "factor", $1->getDataType());
		printLog("factor : ID LPAREN argument_list RPAREN", $$->getName());
	}
}
	| LPAREN expression RPAREN {
	$$ = new SymbolInfo("(" + $2->getName() + ")", "factor", $2->getDataType());
	printLog("factor : LPAREN expression RPAREN", $$->getName());
}
	| CONST_INT {printLog("factor : CONST_INT", $1->getName());}
	| CONST_FLOAT {printLog("factor : CONST_FLOAT", $1->getName());}
	| variable INCOP {
	$$ = new SymbolInfo($1->getName() + "++", "factor", $1->getDataType());
	printLog("factor : variable INCOP", $$->getName());
}
	| variable DECOP {
	$$ = new SymbolInfo($1->getName() + "--", "factor", $1->getDataType());
	printLog("factor : variable DECOP", $$->getName());
}
	;
	
argument_list : arguments {printLog("argument_list : arguments", $1->getName());}
			  | { $$ = new SymbolInfo("", "argument_list");
		 		printLog("argument_list : ", $$->getName());
			  }
			  ;
	
arguments : arguments COMMA logic_expression {
	argumentTypes.push_back($3->getDataType());
	$$ = new SymbolInfo($1->getName() + ", " + $3->getName(), "arguments");
	printLog("arguments : arguments COMMA logic_expression", $$->getName());
}
	      | logic_expression {
	      argumentTypes.push_back($1->getDataType());
	      printLog("arguments : logic_expression", $1->getName());
}
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


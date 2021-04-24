%{
#include <bits/stdc++.h>
#include "SymbolTable.cpp"

using namespace std;

int yyparse(void);
int yylex(void);

FILE* fp;
ofstream log_;
ofstream error;
bool hasScopeStarted;
vector<pair<string, bool>> declaration_list; // bool value determines if its an array
vector<string> argumentTypes;
vector<string> undefinedFunctions;

extern FILE* yyin;
extern int line_count;
extern int error_count;
extern SymbolTable st;


void yyerror(char *s) {
	printf("%s\n", s);
}

void printLog(string grammar, string sentence) {
	log_ << "Line " << line_count << ": " << grammar << "\n\n" << sentence << "\n\n";
}

void handleError(string message) {
	error_count ++;
	error << "Error at line " << line_count << ": " << message << "\n\n";
}

vector<string> stringSplit(string s) {
	istringstream ss(s);
	vector<string> words;
    string word;
    while (ss >> word)
    	words.push_back(word);
    return words;
}

vector<string> getParamTypes(string s) {
	replace(s.begin(), s.end(), ',', ' ');
	vector<string> words = stringSplit(s);
	vector<string> paramTypes;
	if(words.size() == 1 && words[0] == "void") return paramTypes; // eg. func(void)
	for(string word: words) {
    	if(word == "int" || word == "float" || word == "void") {
			if(word == "void") {
    			handleError("Parameter can't be void");
    		}
			paramTypes.push_back(word);
    	}
	}
    return paramTypes;
}

void insertParamsInSymbolTable(string s) {
	replace(s.begin(), s.end(), ',', ' ');
	vector<string> words = stringSplit(s);
    string currentDataType;
    for(string word: words) { 	
    	if(word == "int" || word == "float" || word == "void") {
			currentDataType = word;
    	} else {
    		if( !st.insert(new SymbolInfo(word, "ID", currentDataType)) ) {
    			handleError("Multiple declaration of " + word + " in parameter");
    		}
    	}
	}
}

void insertVarsInSymbolTable(string dataType) {
	if(dataType == "void") {
   		handleError("Variable type cannot be void");
    }
	string arrayDataType = dataType + "_array";
	string dtype;
	for(pair<string, bool> x : declaration_list) {
		if(x.second) dtype = arrayDataType;
		else dtype = dataType;
		if(!st.insert(new SymbolInfo(x.first, "ID", dtype))) {
			handleError("Multiple declaration of " + x.first);
		}
	}
	declaration_list.clear();
}

void verifyWithDeclaration(string returnType, SymbolInfo* funcInfo, vector<string> paramTypes = {}) {
	string name = funcInfo->getName();
	if(returnType != funcInfo->getDataType()) {
		handleError("Return type mismatch with function declaration in function " + name); return;
	}
	vector<string> declaredParamTypes = funcInfo->getParamTypes();
	if(paramTypes.size() != declaredParamTypes.size()) {
		handleError("Total number of arguments mismatch with declaration in function " + name); return;
	}	
	for(int i=0; i<paramTypes.size(); i++) {
		if(paramTypes[i] != declaredParamTypes[i]) {
			handleError(to_string(i+1) + "th argument mismatch in function " + name); return;
		}
	}
}

void checkAndInsertFunc(string returnType, string name, vector<string> paramTypes = {}) {
	SymbolInfo* funcInfo = st.lookUp(name);
	if(funcInfo == NULL) {
		st.insert( new SymbolInfo(name, "ID", returnType, 1, paramTypes) );
	} else {
		if (find(undefinedFunctions.begin(), undefinedFunctions.end(), name) != undefinedFunctions.end()) {
			undefinedFunctions.erase(remove(undefinedFunctions.begin(), undefinedFunctions.end(), name), undefinedFunctions.end());
			verifyWithDeclaration(returnType, funcInfo, paramTypes);
		} else {
			handleError("Multiple Definition of " + name);
		}
	}
}

void verifyFunctionCall(SymbolInfo* funcInfo) {
	vector<string> declaredParamTypes = funcInfo->getParamTypes();
	string name = funcInfo->getName();
	/*for(int i=0; i<argumentTypes.size(); i++) {
		cout << argumentTypes[i] << " ";
	}
	cout << "\n";
	for(int i=0; i<declaredParamTypes.size(); i++) {
		cout << declaredParamTypes[i] << " ";
	}*/
	if(argumentTypes.size() != declaredParamTypes.size()) {
		handleError("Total number of arguments mismatch in function " + name);
	} else {
		for(int i=0; i<argumentTypes.size(); i++) {
			if(argumentTypes[i] != declaredParamTypes[i]) {
				handleError(to_string(i+1) + "th argument mismatch in function " + name); break;
			} else {
				if(argumentTypes[i] == "void") handleError("Invalid use of void expression");
			}
		}	
	}
	argumentTypes.clear();
}

string determineType(string dType1, string dType2) {
	string type;
	if(dType1 == "void" || dType2 == "void") {
		type = "void";
	} else if(dType1 == "" || dType2 == "") {
		type = "";
	} else {
		type = "int";
    	if(dType1 == "float" || dType2 == "float") type = "float";	
	}
	return type;
}

%}

%union {SymbolInfo* x;}
%token <x> IF ELSE FOR WHILE RETURN
%token <x> INT FLOAT VOID
%token <x> ADDOP MULOP INCOP DECOP RELOP ASSIGNOP LOGICOP NOT
%token <x> LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD COMMA SEMICOLON
%token <x> ID PRINTLN CONST_INT CONST_FLOAT 
%type <x> start program unit func_declaration func_definition parameter_list compound_statement var_declaration type_specifier declaration_list statements statement expression_statement variable expression logic_expression rel_expression simple_expression term unary_expression factor argument_list arguments

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

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
		handleError("Multiple declaration of " + name);
	} else {
		undefinedFunctions.push_back(name);
	}
	$$ = new SymbolInfo($1->getName() + " " + name + "(" + $4->getName() + ");", "func_declaration");
	printLog("func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON", $$->getName());
}
		| type_specifier ID LPAREN RPAREN SEMICOLON {
	string name = $2->getName();
	if(!st.insert( new SymbolInfo(name, "ID", $1->getName(), 1) )) {
		handleError("Multiple declaration of " + name);
	} else {
		undefinedFunctions.push_back(name);
	}
	$$ = new SymbolInfo($1->getName() + " " + name + "();", "func_declaration");
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
	//undefinedFunctions.clear();
	st.exitScope();
	printLog("func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement", $$->getName());
}
		| type_specifier ID LPAREN RPAREN {	
	checkAndInsertFunc($1->getName(), $2->getName());
	st.enterScope();
	hasScopeStarted = 1;	
} compound_statement {
	$$ = new SymbolInfo($1->getName() + " " + $2->getName() + "()" + $6->getName(), "func_definition");
	//undefinedFunctions.clear();
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
	$$ = new SymbolInfo("for(" + $3->getName() + $4->getName() + $5->getName() + ")" + $7->getName(), "statement");
	printLog("statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement", $$->getName());
}
	  | IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE {
	$$ = new SymbolInfo("if(" +  $3->getName() + ")" + $5->getName(), "statement");
	printLog("statement : IF LPAREN expression RPAREN statement", $$->getName());
}
	  | IF LPAREN expression RPAREN statement ELSE statement {
	$$ = new SymbolInfo("if(" +  $3->getName() + ")" + $5->getName() + " else " + $7->getName(), "statement");
	printLog("statement : IF LPAREN expression RPAREN statement ELSE statement", $$->getName());
}
	  | WHILE LPAREN expression RPAREN statement {
	$$ = new SymbolInfo("while(" +  $3->getName() + ")" + $5->getName(), "statement");
	printLog("statement : WHILE LPAREN expression RPAREN statement", $$->getName());
}
	  | PRINTLN LPAREN ID RPAREN SEMICOLON {
	string name = $3->getName();
	SymbolInfo* temp = st.lookUp(name);
	if(temp == NULL) {
		handleError("Undeclared variable " + name);
	} else if(temp->getIsFunction()) {
		handleError(name + " is a function, not a variable");
	}
	$$ = new SymbolInfo("printf(" +  name + ");", "statement");
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
	string dataType = "";
	SymbolInfo* temp = st.lookUp(name);
	if(temp == NULL) {
		handleError("Undeclared variable " + name);
	} else {
		if(temp->getIsFunction())
			handleError(name + " is a function, not a variable"); // gotta write it in more places
		else {
			dataType = temp->getDataType();
		}
		if (dataType.find("array") != string::npos) {
			//dataType.substr(0, dataType.find("_"));
			//handleError("Array without index");
		}
	}
	$$ = new SymbolInfo(name, "variable", dataType);
	printLog("variable : ID", name);	
}
	 | ID LTHIRD expression RTHIRD {
	string name = $1->getName();
	string dataType = "";
	if($3->getDataType() != "int") {
		handleError("Expression inside third brackets not an integer");
	} else {
		SymbolInfo* temp = st.lookUp(name);
		if(temp == NULL) {
			handleError("Undeclared variable " + name);
		} else {
			if(temp->getIsFunction())
				handleError(name + " is a function, not a varaible");
			else {
				dataType = temp->getDataType();
				if (dataType.find("array") != string::npos) {
					dataType = dataType.substr(0, dataType.find("_"));
				} else {
					handleError(name + " not an array");
				}
			}
		}	
	}
	$$ = new SymbolInfo(name + "[" + $3->getName() + "]", "variable", dataType);
	printLog("variable : ID LTHIRD expression RTHIRD", $$->getName());
}
	 ;
	 
expression : logic_expression {printLog("expression : logic_expression", $1->getName());}
	   | variable ASSIGNOP logic_expression {
	string lhs_dataType = $1->getDataType();
	string rhs_dataType = $3->getDataType();
	cout << line_count << " " << lhs_dataType << " " << rhs_dataType << "\n";
	if(lhs_dataType == "void" || rhs_dataType == "void") {
		handleError("Void function used in expression");
	}
 	else if(!((lhs_dataType == rhs_dataType)
		|| (lhs_dataType == "" || rhs_dataType == "")
		|| (lhs_dataType == "float" && rhs_dataType == "int"))) {
		handleError("Type Mismatch");
	}	
	$$ = new SymbolInfo($1->getName() + "=" + $3->getName(), "expression");
	printLog("expression : variable ASSIGNOP logic_expression", $$->getName());	
}
	   ;

logic_expression : rel_expression {printLog("logic_expression : rel_expression", $1->getName());}
		 | rel_expression LOGICOP rel_expression {
	string type = "int";
	if($1->getDataType() == "void" || $1->getDataType() == "void") {
		handleError("Type Mismatch");
		type = "";
	}
	$$ = new SymbolInfo($1->getName() + $2->getName() + $3->getName(), "logic_expression", type);
	printLog("logic_expression : rel_expression LOGICOP rel_expression", $$->getName());
}
		 ;

rel_expression : simple_expression {printLog("rel_expression : simple_expression", $1->getName());}
		| simple_expression RELOP simple_expression {
	string type = "int";
	if($1->getDataType() == "void" || $1->getDataType() == "void") {
		handleError("Type Mismatch");
		type = "";
	}
	$$ = new SymbolInfo($1->getName() + $2->getName() + $3->getName(), "rel_expression", type);
	printLog("rel_expression : simple_expression RELOP simple_expression", $$->getName());
}
		;

simple_expression : term {printLog("simple_expression : term", $1->getName());}
		  | simple_expression ADDOP term {
	$$ = new SymbolInfo($1->getName() + $2->getName() + $3->getName(), "simple_expression", determineType($1->getDataType(), $3->getDataType()));
	printLog("simple_expression : simple_expression ADDOP term", $$->getName());
}
		  ;

term :	unary_expression {printLog("term : unary_expression", $1->getName());}
     |  term MULOP unary_expression {
    string type1 = $1->getDataType();
    string op = $2->getName();
    string type2 = $3->getDataType();
    string type = ""; 
    if(op == "%" && (type1 != "int" || type2 != "int")) {
    	handleError("Non-Integer operand on modulus operator");
    } else if(op == "%" && $3->getName() == "0") {
    	handleError("Modulus by Zero");
    } else {
    	type = determineType(type1, type2);
    }
    $$ = new SymbolInfo($1->getName() + $2->getName() + $3->getName(), "term", type);
	printLog("term : term MULOP unary_expression", $$->getName());
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
	string dataType = "";
	SymbolInfo* temp = st.lookUp(name);
	if(temp == NULL) {
		handleError("Undeclared function " + name);
	} else {
		verifyFunctionCall(temp);
		dataType = temp->getDataType();
	}
	$$ = new SymbolInfo(name + "(" + $3->getName() + ")", "factor", dataType);
	printLog("factor : ID LPAREN argument_list RPAREN", $$->getName());
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
	$$ = new SymbolInfo($1->getName() + "," + $3->getName(), "arguments");
	printLog("arguments : arguments COMMA logic_expression", $$->getName());
}
	      | logic_expression {
	      argumentTypes.push_back($1->getDataType());
	      printLog("arguments : logic_expression", $1->getName());
}
	      ;
 

%%
int main(int argc,char *argv[]) {
	// PRINTLN, DECOP ei token gula baki
	if((fp=fopen(argv[1],"r")) == NULL) {
		printf("Cannot Open Input File.\n");
		exit(1);
	}

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
	log_ << "Total Errors: " << error_count; error << "Total Errors: " << error_count;
	fclose(yyin);
	log_.close();
	error.close();
	
	return 0;
}


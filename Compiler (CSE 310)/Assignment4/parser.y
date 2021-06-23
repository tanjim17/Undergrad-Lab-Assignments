%{
#include <bits/stdc++.h>
#include "SymbolTable.cpp"

using namespace std;

int yyparse(void);
int yylex(void);

FILE* fp;
ofstream log_;
ofstream error;
ofstream asmFile;
bool hasScopeStarted;
vector<pair<string, string>> declaration_list; // second element is size of array ("0" will be stored for variables)
vector<string> argumentTypes;
vector<string> undefinedFunctions;

//these variables are for code generation
vector<string> variables;
vector<string> arrays;
//

extern FILE* yyin;
extern int line_count;
extern int error_count;
extern SymbolTable st;

void handleError(string message) {
	string fullMessage = "Error at line " + to_string(line_count) + ": " + message + "\n\n";
	error << fullMessage;
	log_ << fullMessage;
	error_count ++;
}

void yyerror(string s) {
	handleError(s);
}

void printLog(string grammar, string sentence) {
	log_ << "Line " << line_count << ": " << grammar << "\n\n" << sentence << "\n\n";
}

vector<string> stringSplit(string s) {
	istringstream ss(s);
	vector<string> words;
    string word;
    while (ss >> word)
    	words.push_back(word);
    return words;
}

vector<vector<string>> extractParamInfos(string s) {
	replace(s.begin(), s.end(), ',', ' ');
	vector<string> param_infos = stringSplit(s);
	vector<string> paramNames;
	vector<string> paramTypes;
	if(param_infos.size() == 1 && param_infos[0] == "void") return paramTypes; // eg. func(void)
	for(string param_info: param_infos) {
    	if(param_info == "int" || param_info == "float" || param_info == "void") {
			if(param_info == "void") {
    			handleError("Parameter can't be void");
    		}
			paramTypes.push_back(param_info);
    	} else {
    		paramNames.push_back(param_info);
    	}
	}
	vector<vector<string>> extractedParamInfos;
	extractedParamInfos.push_back(paramNames);
	extractedParamInfos.push_back(paramTypes);
    return extractedParamInfos;
}

void insertParams(vector<string> paramNames) {
    string currentDataType;
    for(string paramName: paramNames) { 	
    	if( !st.insert(new SymbolInfo(param_info, "ID", currentDataType)) ) {
    			handleError("Multiple declaration of " + param_info + " in parameter");
    	} else {
    		variables.push_back(param_info + st.getCurrentScopeId());
    	}
	}
}

void insertVars(string dataType) {
	if(dataType == "void") {
   		handleError("Variable type cannot be void");
    }
	string arrayDataType = dataType + "_array";
	string dtype;
	for(pair<string, int> x : declaration_list) {
		if(x.second != "0") dtype = arrayDataType;
		else dtype = dataType;
		if(!st.insert(new SymbolInfo(x.first, "ID", dtype))) {
			handleError("Multiple declaration of " + x.first);
		} else {
			if(x.second != "0") arrays.push_back(x.first + st.getCurrentScopeId(), x.second);
			else variables.push_back(x.first + st.getCurrentScopeId());
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
		type = ""; // empty type means an error has already been reported
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
	string code = ".MODEL SMALL\n.STACK 100H\n.DATA\n";
	
	for(int i = 0; i < variables.size(); i++) {
		code += variables[i] + " DW ?\n";
	}
	for(int i = 0; i < arrays.size(); i++) {
		code+=arrays[i].first + " DW " + arrays[i].second + " DUP(?)\n";
	}
	
	code += ".CODE\n" + $1->getCode();		
		/*string tempCode = "";
		tempCode += "outdec proc\npush ax\npush bx\npush cx\npush dx\n";
			tempCode += "or ax,ax\njge END_IF1\npush ax\nmov dl,'-'\n";
			tempCode += "mov ah,2\nint 21h\npop ax\nneg ax\n";
			tempCode += "END_IF1:\nxor cx,cx\nmov bx,10d\n";
			tempCode += "REPEAT1:\nxor dx,dx\ndiv bx\npush dx\ninc cx\nor ax,ax\njne REPEAT1\n";
			tempCode += "mov ah,2\nPRINT_LOOP:\npop dx\nor dl,30h\nint 21h\nloop PRINT_LOOP\nmov dx,13\nmov ah,2\nint 21h \nmov dx,10\nmov ah,2\nint 21h\n";
			tempCode += "pop dx\npop cx\npop bx\npop ax\nret\noutdec endp\n";
		code += tempCode;*/		
	code += "END MAIN";
  	asmFile << code;
}
	;

program : program unit {
	$$ = new SymbolInfo($1->getName() + "\n" + $2->getName(), "program");
	printLog("program : program unit", $$->getName());
	$$->setCode($1->getCode() + $2->getCode());
}
	| unit {
	$$ = new SymbolInfo($1->getName(), "program");
	printLog("program : unit", $$->getName());
	$$->setCode($1->getCode());
}
	;
	
unit : var_declaration {printLog("unit : var_declaration", $1->getName()); $$->setCode($1->getCode());}
     | func_declaration {printLog("unit : func_declaration", $1->getName()); $$->setCode($1->getCode());}
     | func_definition {printLog("unit : func_definition", $1->getName()); $$->setCode($1->getCode());}
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
	vector<vector<string>> extractedParamInfos = extractedParamInfos($4->getName());
	vector<string> paramNames = extractedParamInfos[0];
	vector<string> paramTypes = extractedParamInfos[1];
	checkAndInsertFunc($1->getName(), $2->getName(), paramTypes);
	st.enterScope();
	insertParams(paramNames);
	variables.push_back($2->getName() + "ReturnVar");
	hasScopeStarted = 1;
} compound_statement {
	$$ = new SymbolInfo($1->getName() + " " + $2->getName() + "(" + $4->getName() + ")" + $7->getName(), "func_definition");
	string code = $2->getName() + " PROC\n";
	if ($2->getName() == "main") {
		$$->setCode(code + "MOV AX, @DATA\nMOV DS, AX\n" + $7->getCode() + "labelReturnMain:\nMOV AH, 4CH\nINT 21H\n"); //change this name "labelReturn" because copy checker!!
	} else {
		code += "PUSH AX\nPUSH BX \nPUSH CX \nPUSH DX\n";
		for(string paramName: paramNames) {
			code += "PUSH " + paramName + st.getCurrentScopeId() + "\n";
		}
		code += $7->getCode() + "labelReturn" + $2->getName() + ":\n";	
		for(string paramName: paramNames) {
			code += "POP " + paramName + st.getCurrentScopeId() + "\n";
		}
		code += "POP DX\nPOP CX\nPOP BX\nPOP AX\nret\n"; 
		$$->setCode(code + $2->getName() + " ENDP\n");
	}
	st.exitScope();
	printLog("func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement", $$->getName());
}
		| type_specifier ID LPAREN RPAREN {	
	checkAndInsertFunc($1->getName(), $2->getName());
	st.enterScope();
	hasScopeStarted = 1;	
	variables.push_back($2->getName() + "ReturnVar");
} compound_statement {
	$$ = new SymbolInfo($1->getName() + " " + $2->getName() + "()" + $6->getName(), "func_definition");
	string code = $2->getName() + " PROC\n";
	if ($2->getName() == "main") {
		$$->setCode(code + "MOV AX, @DATA\nMOV DS, AX\n" + $7->getCode() + "labelReturnMain:\nMOV AH, 4CH\nINT 21H\n"); //change this name "labelReturn" because copy checker!!
	} else {
		code += "PUSH AX\nPUSH BX \nPUSH CX \nPUSH DX\n";
		code += $7->getCode() + "labelReturn" + $2->getName() + ":\n";	
		code += "POP DX\nPOP CX\nPOP BX\nPOP AX\nret\n"; 
		$$->setCode(code + $2->getName() + " ENDP\n");
	}
	st.exitScope();
	printLog("func_definition : type_specifier ID LPAREN RPAREN compound_statement", $$->getName());
}
 		;


parameter_list : parameter_list COMMA type_specifier ID {
	$$ = new SymbolInfo($1->getName() + "," + $3->getName() + " " + $4->getName(), "parameter_list");
	printLog("parameter_list : parameter_list COMMA type_specifier ID", $$->getName());
}
		| parameter_list COMMA type_specifier {
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
	$$->setCode($3->getCode());
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
	$$ = new SymbolInfo($1->getName() + " " + $2->getName() + ";", "var_declaration");
	printLog("var_declaration : type_specifier declaration_list SEMICOLON", $$->getName());
	insertVars($2->getName());
}
 		 ;
 		 
type_specifier : INT {printLog("type_specifier : INT", $1->getName());}
 		| FLOAT {printLog("type_specifier : FLOAT", $1->getName());}
 		| VOID {printLog("type_specifier : VOID", $1->getName());}
 		;
 		
declaration_list : declaration_list COMMA ID {
	$$ = new SymbolInfo($1->getName() + "," + $3->getName(), "declaration_list");
	printLog("declaration_list : declaration_list COMMA ID", $$->getName());
	declaration_list.push_back({$3->getName(), "0"});
}
 		  | declaration_list COMMA ID LTHIRD CONST_INT RTHIRD {
	$$ = new SymbolInfo($1->getName() + "," + $3->getName() + "[" + $5->getName() + "]", "declaration_list");
	printLog("declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD", $$->getName());
	declaration_list.push_back({$3->getName(), $5-getName()});
}
 		  | ID {
 	printLog("declaration_list : ID", $1->getName());
 	declaration_list.push_back({$1->getName(), "0"});
 }
 		  | ID LTHIRD CONST_INT RTHIRD {
	$$ = new SymbolInfo($1->getName() + "[" + $3->getName() + "]", "declaration_list");
	printLog("declaration_list : ID LTHIRD CONST_INT RTHIRD", $$->getName());
	declaration_list.push_back({$1->getName(), $5-getName()});
}
 		  ;
 		  
statements : statement {printLog("statements : statement", $1->getName()); $$->setCode($1->getCode());}
	   | statements statement {
	$$ = new SymbolInfo($1->getName() + "\n" + $2->getName(), "statements");
	printLog("statements : statements statement", $$->getName());
	$$->setCode($1->getCode() + $2->getCode());
}
	   ;
	   
statement : var_declaration {printLog("statement : var_declaration", $1->getName());}
	  | expression_statement {printLog("statement : expression_statement", $1->getName()); $$->setCode($1->getCode());}
	  | compound_statement {printLog("statement : compound_statement", $1->getName()); st.exitScope(); $$->setCode($1->getCode());}
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
			handleError(name + " is a function, not a variable");
		else {
			dataType = temp->getDataType();
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
	//cout << line_count << " " << lhs_dataType << " " << rhs_dataType << "\n";
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
	if($1->getDataType() == "void" || $3->getDataType() == "void") {
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
	if($1->getDataType() == "void" || $3->getDataType() == "void") {
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
	if((fp=fopen(argv[1],"r")) == NULL) {
		printf("Cannot Open Input File.\n");
		exit(1);
	}

	log_.open("log.txt");
	error.open("error.txt");
	asmFile.open("assemblyCode.asm");
	
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


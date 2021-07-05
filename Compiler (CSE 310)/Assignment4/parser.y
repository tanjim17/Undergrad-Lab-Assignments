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
vector<pair<string, string>> arrays;
int labelCount = 0;
int tempCount = 0;
string currentFunc;
vector<SymbolInfo*> argumentSymbolInfos;
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

vector<string> tokenizeAsmInstruction(string s) {
	replace(s.begin(), s.end(), ',', ' ');
	return stringSplit(s);
}

vector<vector<string>> extractParamInfos(string s) {
	replace(s.begin(), s.end(), ',', ' ');
	vector<string> param_infos = stringSplit(s);
	vector<vector<string>> extractedParamInfos;
	for(int i = 0; i < 2; i++) {
		extractedParamInfos.push_back({});	
	} //first vector will contain param names, second one will contain param types

	if(param_infos.size() == 1 && param_infos[0] == "void") return extractedParamInfos; // eg. func(void)
	for(string param_info: param_infos) {
    	if(param_info == "int" || param_info == "float" || param_info == "void") {
			if(param_info == "void") {
    			handleError("Parameter can't be void");
    		}
			extractedParamInfos[1].push_back(param_info);
    	} else {
    		extractedParamInfos[0].push_back(param_info);
    	}
	}
    return extractedParamInfos;
}

void insertParams(vector<vector<string>> extractedParamInfos, SymbolInfo* funcInfo) {
	string paramName;
    string dataType;
    string asmName;
    vector<string> paramAsmNames;
    for(int i = 0; i < extractedParamInfos[0].size(); i++) {
    	paramName = extractedParamInfos[0][i];
    	dataType = extractedParamInfos[1][i];
    	SymbolInfo* symbolInfo = new SymbolInfo(paramName, "ID", dataType);
    	asmName = paramName + st.getCurrentScopeId();
    	paramAsmNames.push_back(asmName);
    	symbolInfo->setAsmName(asmName);
    	if(!st.insert(symbolInfo)) {
    			handleError("Multiple declaration of " + paramName + " in parameter");
    	} else {
    		variables.push_back(asmName);
    	}
	}
	//for(string paramAsmName: paramAsmNames) {cout << paramAsmName;}
	funcInfo->setParamAsmNames(paramAsmNames);
}

void insertVars(string dataType) {
	if(dataType == "void") {
   		handleError("Variable type cannot be void");
    }
	string arrayDataType = dataType + "_array";
	string dtype;
	string asmName;
	for(pair<string, string> x : declaration_list) {
		if(x.second != "0") dtype = arrayDataType;
		else dtype = dataType;
		SymbolInfo* symbolInfo = new SymbolInfo(x.first, "ID", dtype);
		asmName = x.first + st.getCurrentScopeId();
    	symbolInfo->setAsmName(asmName);
		if(!st.insert(symbolInfo)) {
			handleError("Multiple declaration of " + x.first);
		} else {
			if(x.second != "0") arrays.push_back({asmName, x.second});
			else variables.push_back(asmName);
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

SymbolInfo* checkAndInsertFunc(string returnType, string name, vector<string> paramTypes = {}) {
	SymbolInfo* funcInfo = st.lookUp(name);
	if(funcInfo == NULL) {
		funcInfo = new SymbolInfo(name, "ID", returnType, 1);
		funcInfo->setParamTypes(paramTypes);
		st.insert(funcInfo);
	} else {
		if (find(undefinedFunctions.begin(), undefinedFunctions.end(), name) != undefinedFunctions.end()) {
			undefinedFunctions.erase(remove(undefinedFunctions.begin(), undefinedFunctions.end(), name), undefinedFunctions.end());
			verifyWithDeclaration(returnType, funcInfo, paramTypes);
		} else {
			handleError("Multiple Definition of " + name);
		}
	}
	return funcInfo;
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
	cout << argumentTypes.size() << " " << declaredParamTypes.size() << " yo\n";
	if(argumentTypes.size() != declaredParamTypes.size()) {
		handleError("Total number of arguments mismatch in function " + name);
	} else {
		for(int i=0; i<argumentTypes.size(); i++) {
			cout << argumentTypes[i] << " " << declaredParamTypes[i] << " " << name << "\n";
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

string newLabel() {
	return "Label_" + to_string(labelCount++);
}

string newTemp() {
	string temp = "temp_" + to_string(tempCount++);
    variables.push_back(temp);
	return temp;
}

/*static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

string findCmd(string line) {
	int i = 0;
	int len = line.size();
	for(i = 0; line[i]!=' '; i++) {
		if(i >= len) {
			return "";
		}
	}
	return line.substr(0,i);
}


string findDestArg(string line) {
	int i=0;
	int len = line.size();
	for(i = 0; line[i]!=' '; i++) {
		if(i >= len) {
			return "";
		}
	}
	i++;
	int j = i + 1;
	for( ; line[j] != ',' ; j++) {	
		if(j >= line.size()) {
			return "";
		}
	}
	return trim_copy(line.substr(i, j-i));
}


string findSrcArg(string line) {
	int len = line.size();
	int i = 0;
	for(i = 0; line[i]!= ','; i++) {
		if(i >= len) {
			return "";
		}
	}
	i += 1;
	return trim_copy(line.substr(i, len-i));
}*/

void peepholeOptimize() {
	ifstream in("code.asm");
	ofstream out("optimized_code.asm");

	string prevLine = "";
	while(1) {
		getline(in, prevLine);	
		if(!in) {
			in.close();
			out.close();
			return;
		}
		if(prevLine != "") break;
	}
	out << prevLine << "\n";
	
	string currLine = "";
	while(1) {
		getline(in, currLine);	
		if(!in) {
			in.close();
			out.close();
			return;
		}
		if(currLine == "") continue;
		// cout << "\nLine: " << currLine << endl;
		// cout << "CMD: " << findCmd(currLine) << endl;
		// cout << "Source Arg: " << findSrcArg(currLine) << endl;
		// cout << "Dest Arg: " << findDestArg(currLine) << "\n\n";
		vector<string> prevTokens = tokenizeAsmInstruction(prevLine);
		vector<string> currTokens = tokenizeAsmInstruction(currLine);
		if(prevTokens[0] == "MOV" && currTokens[0] == "MOV") {	
			if(prevTokens[1] == currTokens[2] && prevTokens[2] == currTokens[1]) {
				out << ";line deleted here (peephole optimization)\n";
				continue;
			}
		}	
		out << currLine << endl;
		prevLine = currLine;
	}
	return;
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
		code += arrays[i].first + " DW " + arrays[i].second + " DUP(?)\n";
	}
	
	code += string("\n.CODE\nPRINT_INT PROC\n") +
    "PUSH AX\n\PUSH BX\n\PUSH CX\nPUSH DX\n" +
 	"CMP AX, 0\nJGE START\nPUSH AX\nMOV AH, 2\nMOV DL, '-'\nINT 21H\nPOP AX\nNEG AX\n" +
    "START:\nXOR CX, CX\nMOV BX, 10\n" +
    "EXTRACT_DIGIT:\nXOR DX, DX\nDIV BX\nPUSH DX\nINC CX\nOR AX, AX\nJNE EXTRACT_DIGIT\n" +
    "MOV AH, 2\nPRINT:\nPOP DX\nADD DL, 30H\nINT 21H\nLOOP PRINT\n" +
    "MOV DX, 13\nMOV AH, 2\nINT 21h\nMOV DX, 10\nMOV AH, 2\nINT 21h\n" +
    "POP DX\nPOP CX\nPOP BX\nPOP AX\n" +
    "RET\nPRINT_INT ENDP\n\n";
	code += $1->getCode();	
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
	vector<vector<string>> extractedParamInfos = extractParamInfos($4->getName());
	SymbolInfo* symbolInfo = new SymbolInfo(name, "ID", $1->getName(), 1);
	symbolInfo->setParamTypes(extractedParamInfos[1]);
	if(!st.insert(symbolInfo)) {
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
	vector<vector<string>> extractedParamInfos = extractParamInfos($4->getName());
	vector<string> paramNames = extractedParamInfos[0];
	vector<string> paramTypes = extractedParamInfos[1];
	SymbolInfo* funcInfo = checkAndInsertFunc($1->getName(), $2->getName(), paramTypes);
	st.enterScope();
	insertParams(extractedParamInfos, funcInfo);
	variables.push_back($2->getName() + "ReturnVar");
	currentFunc = $2->getName();
	hasScopeStarted = 1;
} compound_statement {
	$$ = new SymbolInfo($1->getName() + " " + $2->getName() + "(" + $4->getName() + ")" + $7->getName(), "func_definition");
	vector<vector<string>> extractedParamInfos = extractParamInfos($4->getName());
	vector<string> paramNames = extractedParamInfos[0];
	string code = $2->getName() + " PROC\n";
	if ($2->getName() == "main") {
		$$->setCode(code + "MOV AX, @DATA\nMOV DS, AX\n" + $7->getCode() + "mainReturn:\nMOV AH, 4CH\nINT 21H\n");
	} else {
		code += "PUSH AX\nPUSH BX \nPUSH CX \nPUSH DX\n";
		for(string paramName: paramNames) {
			code += "PUSH " + paramName + st.getCurrentScopeId() + "\n";
		}
		code += $7->getCode() + $2->getName() + "Return" + ":\n";
		for(int i = paramNames.size() - 1; i >= 0; i--) {
			code += "POP " + paramNames[i] + st.getCurrentScopeId() + "\n";
		}
		code += "POP DX\nPOP CX\nPOP BX\nPOP AX\nRET\n"; 
		$$->setCode(code + $2->getName() + " ENDP\n\n");
	}
	st.exitScope();
	printLog("func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement", $$->getName());
}
		| type_specifier ID LPAREN RPAREN {	
	checkAndInsertFunc($1->getName(), $2->getName());
	st.enterScope();
	hasScopeStarted = 1;
	variables.push_back($2->getName() + "ReturnVar");
	currentFunc = $2->getName();
} compound_statement {
	$$ = new SymbolInfo($1->getName() + " " + $2->getName() + "()" + $6->getName(), "func_definition");
	string code = $2->getName() + " PROC\n";
	if ($2->getName() == "main") {
		$$->setCode(code + "MOV AX, @DATA\nMOV DS, AX\n" + $6->getCode() + "mainReturn:\nMOV AH, 4CH\nINT 21H\n");
	} else {
		code += "PUSH AX\nPUSH BX \nPUSH CX \nPUSH DX\n";
		code += $6->getCode() + $2->getName() + "Return" + ":\n";
		code += "POP DX\nPOP CX\nPOP BX\nPOP AX\nRET\n"; 
		$$->setCode(code + $2->getName() + " ENDP\n\n");
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
	insertVars($1->getName());
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
	declaration_list.push_back({$3->getName(), $5->getName()});
}
 		  | ID {
 	printLog("declaration_list : ID", $1->getName());
 	declaration_list.push_back({$1->getName(), "0"});
 }
 		  | ID LTHIRD CONST_INT RTHIRD {
	$$ = new SymbolInfo($1->getName() + "[" + $3->getName() + "]", "declaration_list");
	printLog("declaration_list : ID LTHIRD CONST_INT RTHIRD", $$->getName());
	declaration_list.push_back({$1->getName(), $3->getName()});
}
 		  ;
 		  
statements : statement {
	printLog("statements : statement", $1->getName());
	$$->setCode($1->getCode());
}
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
	string loopStartLabel = newLabel();
	string loopEndLabel = newLabel();
	string code = ";loop init\n" + $3->getCode() +
	loopStartLabel + ":\n;condition\n" + $4->getCode() +
	"MOV AX, " + $4->getAsmName() + "\nCMP AX, 0\nJE " + loopEndLabel + "\n" +
	";loop statement\n" + $7->getCode() + ";increment\n" + $5->getCode() + 
	"JMP " + loopStartLabel + "\n" + loopEndLabel + ":\n" + ";loop end\n";
	$$->setCode(code);
}
	  | IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE {
	$$ = new SymbolInfo("if(" +  $3->getName() + ")" + $5->getName(), "statement");
	printLog("statement : IF LPAREN expression RPAREN statement", $$->getName());
	string endIfLabel = newLabel();
	string code = ";if\n" + $3->getCode() + 
	"MOV AX, " + $3->getAsmName() + "\nCMP AX, 0\nJE " + endIfLabel + "\n" +
	";then\n" + $5->getCode() + endIfLabel + ":\n;endif\n";
	$$->setCode(code);
}
	  | IF LPAREN expression RPAREN statement ELSE statement {
	$$ = new SymbolInfo("if(" +  $3->getName() + ")" + $5->getName() + " else " + $7->getName(), "statement");
	printLog("statement : IF LPAREN expression RPAREN statement ELSE statement", $$->getName());
	string elseLabel = newLabel();
	string endIfLabel = newLabel();
	string code = ";if\n" + $3->getCode() + 
	"MOV AX, " + $3->getAsmName() + "\nCMP AX, 0\nJE " + elseLabel + "\n" +
	";then\n" + $5->getCode() + "JMP " + endIfLabel + "\n" +
	";else\n" + elseLabel + ":\n" + $7->getCode() + endIfLabel + ":\n;endif\n";
	$$->setCode(code);
}
	  | WHILE LPAREN expression RPAREN statement {
	$$ = new SymbolInfo("while(" +  $3->getName() + ")" + $5->getName(), "statement");
	printLog("statement : WHILE LPAREN expression RPAREN statement", $$->getName());
	string loopStartLabel = newLabel();
	string loopEndLabel = newLabel();
	string code = ";loop start\n" + loopStartLabel + ":\n;condition\n" + $3->getCode() + 
	"MOV AX, " + $3->getAsmName() + "\n" + "CMP AX, 0\n" + "JE " + loopEndLabel + "\n" +
	";loop statement\n" + $5->getCode() +
	"JMP " + loopStartLabel + "\n" + loopEndLabel + ":\n;loop end\n";
	$$->setCode(code);
}
	  | PRINTLN LPAREN ID RPAREN SEMICOLON {
	string name = $3->getName();
	SymbolInfo* temp = st.lookUp(name);
	if(temp == NULL) {
		handleError("Undeclared variable " + name);
	} else if(temp->getIsFunction()) {
		handleError(name + " is a function, not a variable");
	}
	$$ = new SymbolInfo($1->getName() + "(" +  name + ");", "statement");
	printLog("statement : PRINTLN LPAREN ID RPAREN SEMICOLON", $$->getName());
	SymbolInfo* symbolInfo = st.lookUp($3->getName());
	string code = "MOV AX, " + symbolInfo->getAsmName() + "\nCALL PRINT_INT\n";
	$$->setCode(code);
}
	  | RETURN expression SEMICOLON {
	$$ = new SymbolInfo("return " +  $2->getName() + ";", "statement");
	printLog("statement : RETURN expression SEMICOLON", $$->getName());
	string code = $2->getCode();
	code += "MOV AX, " + $2->getAsmName() + "\nMOV " + currentFunc + "ReturnVar, AX\n";
	code += "JMP " + currentFunc + "Return\n";
	$$->setCode(code);
}
	  ;
	  
expression_statement : SEMICOLON {printLog("expression_statement : SEMICOLON", $1->getName());}
			| expression SEMICOLON {
	$$ = new SymbolInfo($1->getName() + ";", "expression_statement");
	printLog("expression_statement : expression SEMICOLON", $$->getName());
	$$->setCode($1->getCode());
	$$->setAsmName($1->getAsmName());
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
	if(temp != NULL) {
		$$->setAsmName(temp->getAsmName());
	}
}
	 | ID LTHIRD expression RTHIRD {
	string name = $1->getName();
	string dataType = "";
	SymbolInfo* temp;
	if($3->getDataType() != "int") {
		handleError("Expression inside third brackets not an integer");
	} else {
		temp = st.lookUp(name);
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
	string code = $3->getCode() + "MOV CX, " + $3->getAsmName() + "\nADD CX, CX\n";
	//$$->setAssemblyArrayMember(true); // ?
	$$->setIsArray(true);
	$$->setCode(code);
	if(temp != NULL) {
		$$->setAsmName(temp->getAsmName());	
	}
}
	 ;
	 
expression : logic_expression {
	printLog("expression : logic_expression", $1->getName());
	$$->setCode($1->getCode());
	$$->setAsmName($1->getAsmName());
}
	   | variable ASSIGNOP logic_expression {
	string lhs_dataType = $1->getDataType();
	string rhs_dataType = $3->getDataType();
	if(lhs_dataType == "void" || rhs_dataType == "void") {
		handleError("Void function used in expression");
	}
 	else if(!((lhs_dataType == lhs_dataType)
		|| (lhs_dataType == "" || rhs_dataType == "")
		|| (lhs_dataType == "float" && rhs_dataType == "int"))) {
		handleError("Type Mismatch");
	}
	string code = $1->getCode() + $3->getCode();
	code += "MOV AX, " + $3->getAsmName() + "\n";
	if($1->getIsArray()) {
		code += "MOV BX, CX\n";
		code += "MOV " + $1->getAsmName() + "[BX], AX\n";
	} else {
		code += "MOV " + $1->getAsmName() + ", AX\n";
	}	
	$$ = new SymbolInfo($1->getName() + "=" + $3->getName(), "expression");
	printLog("expression : variable ASSIGNOP logic_expression", $$->getName());
	$$->setCode(code);
	$$->setAsmName($1->getAsmName());
}
	   ;

logic_expression : rel_expression {
	printLog("logic_expression : rel_expression", $1->getName());
	$$->setCode($1->getCode());
	$$->setAsmName($1->getAsmName());
}
		 | rel_expression LOGICOP rel_expression {
	string type = "int";
	if($1->getDataType() == "void" || $3->getDataType() == "void") {
		handleError("Type Mismatch");
		type = "";
	}
	$$ = new SymbolInfo($1->getName() + $2->getName() + $3->getName(), "logic_expression", type);
	printLog("logic_expression : rel_expression LOGICOP rel_expression", $$->getName());
	string code = $1->getCode() + $3->getCode() + ";logicalExpressionStart\n";
	string temp = newTemp();
	$$->setAsmName(temp);
	string skipConditionLabel = newLabel();
	string expressionEndLabel = newLabel();
    if ($2->getName() == "||") {
		code += "MOV AX, " + $1->getAsmName() + "\nCMP AX, 0\nJNE " + skipConditionLabel + "\n" +
		"MOV AX, " + $3->getAsmName() + "\nCMP AX, 0\nJNE " + skipConditionLabel + "\n" +
		";ifExpressionFalse\nMOV " + temp + ", 0\nJMP " + expressionEndLabel + "\n" +
		skipConditionLabel + ":\n;ifExpressionTrue\nMOV " + temp + ", 1\n" +
		expressionEndLabel + ":\n;endExpression\n";
	} else if ($2->getName() == "&&") {
		code += "MOV AX, " + $1->getAsmName() + "\nCMP AX, 0\nJE " + skipConditionLabel + "\n" +
		"MOV AX, " + $3->getAsmName() + "\nCMP AX, 0\nJE " + skipConditionLabel + "\n" +
		";ifExpressionTrue\nMOV " + temp + ", 1\nJMP " + expressionEndLabel + "\n" +
		skipConditionLabel + ":\n;ifExpressionFalse\nMOV " + temp + ", 0\n" +
		expressionEndLabel + ":\n;logicalExpressionStart\n";
	}
	$$->setCode(code);
}
		 ;

rel_expression : simple_expression {
	printLog("rel_expression : simple_expression", $1->getName());
	$$->setCode($1->getCode());
	$$->setAsmName($1->getAsmName());
}
		| simple_expression RELOP simple_expression {
	string type = "int";
	if($1->getDataType() == "void" || $3->getDataType() == "void") {
		handleError("Type Mismatch");
		type = "";
	}
	$$ = new SymbolInfo($1->getName() + $2->getName() + $3->getName(), "rel_expression", type);
	printLog("rel_expression : simple_expression RELOP simple_expression", $$->getName());
	string temp = newTemp();
	$$->setAsmName(temp);
	string code = $1->getCode() + $3->getCode();
	string trueLabel = newLabel();
	string relopEndLabel = newLabel();
	code += "MOV AX, " + $1->getAsmName() + "\nCMP AX, " + $3->getAsmName() + "\n";	
	string op = $2->getName();
    if (op == "<") {
		code += "JL " + trueLabel + "\n";
	} else if (op == ">") {
		code += "JG " + trueLabel + "\n";
	} else if (op == "<=") {
		code += "JLE " + trueLabel + "\n";
	} else if (op == ">=") {
		code += "JGE " + trueLabel + "\n";
	} else if (op == "==") {
		code += "JE " + trueLabel + "\n";
	} else if (op == "!=") {
		code += "JNE " + trueLabel + "\n";
	}
	code += "MOV " + temp + ", 0\nJMP " + relopEndLabel + "\n" +
	trueLabel + ":\nMOV " + temp + ", 1\n" + relopEndLabel + ":\n";
	$$->setCode(code);
}
		;

simple_expression : term {
	printLog("simple_expression : term", $1->getName());
	$$->setCode($1->getCode());
	$$->setAsmName($1->getAsmName());
}
		  | simple_expression ADDOP term {
	$$ = new SymbolInfo($1->getName() + $2->getName() + $3->getName(), "simple_expression", determineType($1->getDataType(), $3->getDataType()));
	printLog("simple_expression : simple_expression ADDOP term", $$->getName());
	string code = $1->getCode() + $3->getCode();
	code += "MOV AX, " + $1->getAsmName() + "\n";
	string temp = newTemp();
	if ($2->getName() == "+") {
		code += "ADD AX, " + $3->getAsmName() + "\n";
	} else {
		code += "SUB AX, " + $3->getAsmName() + "\n";
	}
	code += "MOV " + temp + ", AX\n";
	$$->setCode(code);
	$$->setAsmName(temp);
}
		  ;

term :	unary_expression {
	printLog("term : unary_expression", $1->getName());
	$$->setCode($1->getCode());
	$$->setAsmName($1->getAsmName());
}
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
	string code = $1->getCode() + $3->getCode();
	string temp = newTemp();
	code += "MOV AX, " + $1->getAsmName() + "\nMOV BX, " + $3->getAsmName() + "\n";
	if(op == "%") {
		code += "MOV DX, 0\nDIV BX\nMOV " + temp + ", DX\n";	
	} else {
		if(op == "*") code += "MUL BX\n";
		else if(op == "/") code += "DIV BX\n";
		code += "MOV " + temp + ", AX\n";
	}
	$$->setCode(code);
	$$->setAsmName(temp);
}
     ;

unary_expression : ADDOP unary_expression {
	$$ = new SymbolInfo($1->getName() + $2->getName(), "unary_expression", $2->getDataType());
	printLog("unary_expression : ADDOP unary_expression", $$->getName());
	string code = $2->getCode();
	if($1->getName() == "-") {
		code += "MOV AX, " + $2->getAsmName() + "\nNEG AX\n" +
		"MOV " + $2->getAsmName() + ", AX\n";
	} 
	$$->setCode(code);
	$$->setAsmName($2->getAsmName());
}
		 | NOT unary_expression {
	$$ = new SymbolInfo("!" + $2->getName(), "unary_expression", $2->getDataType());
	printLog("unary_expression : NOT unary_expression", $$->getName());
	string code = $2->getCode();
		code += "MOV AX, " + $2->getAsmName() + "\nNOT AX\n" +
		"MOV " + $2->getAsmName() + ", AX\n";
	$$->setCode(code);
	$$->setAsmName($2->getAsmName());
}
		 | factor {
	printLog("unary_expression : factor", $1->getName());
	$$->setCode($1->getCode());
	$$->setAsmName($1->getAsmName());
}
		 ;
	
factor : variable {
	printLog("factor : variable", $1->getName());
	string code = $1->getCode();
	if($1->getIsArray()) {
		string temp = newTemp();
		code += "MOV BX, CX\n";
		code += "MOV AX, " + $1->getAsmName() + "[BX]\n";
		code += "MOV " + temp + ", AX\n";
		$$->setAsmName(temp);
	} else {
		$$->setAsmName($1->getAsmName());
	}
	$$->setCode(code);
}
	| ID LPAREN argument_list RPAREN {
	string name = $1->getName();
	string dataType = "";
	SymbolInfo* funcInfo = st.lookUp(name);
	if(funcInfo == NULL) {
		handleError("Undeclared function " + name);
	} else {
		verifyFunctionCall(funcInfo);
		dataType = funcInfo->getDataType();
	}
	string code = $3->getCode();
	vector<string> paramAsmNames = funcInfo->getParamAsmNames();
	//reverse(paramAsmNames.begin(), paramAsmNames.end());
	//cout << paramAsmNames.size();
	for(int i = 0 ; i < paramAsmNames.size(); i++) {
		code += "MOV AX, " + argumentSymbolInfos[i]->getAsmName() + "\n";
		code += "MOV " + paramAsmNames[i] + ", AX\n";

	}
	code += "CALL " + $1->getName() + "\n";
	code += "MOV AX, " + $1->getName() + "ReturnVar\n";
	string temp = newTemp();
	code += "MOV " + temp + ", AX\n";
	argumentSymbolInfos.clear();
	$$ = new SymbolInfo(name + "(" + $3->getName() + ")", "factor", dataType);
	printLog("factor : ID LPAREN argument_list RPAREN", $$->getName());
	$$->setCode(code);
	$$->setAsmName(temp);
}
	| LPAREN expression RPAREN {
	$$ = new SymbolInfo("(" + $2->getName() + ")", "factor", $2->getDataType());
	printLog("factor : LPAREN expression RPAREN", $$->getName());
	$$->setCode($2->getCode());
	$$->setAsmName($2->getAsmName());
}
	| CONST_INT {
	printLog("factor : CONST_INT", $1->getName());
	string temp = newTemp();
	string code = "MOV " + temp + ", " + $1->getName() + "\n";
	$$->setAsmName(temp);
	$$->setCode(code);
}
	| CONST_FLOAT {
	printLog("factor : CONST_FLOAT", $1->getName());
	string temp = newTemp();
	string code = "MOV " + temp + ", " + $1->getName() + "\n";
	$$->setAsmName(temp);
	$$->setCode(code);
}
	| variable INCOP {
	$$ = new SymbolInfo($1->getName() + "++", "factor", $1->getDataType());
	printLog("factor : variable INCOP", $$->getName());
	string temp = newTemp();
	string code = "";
	if($$->getIsArray()) {
		code += "MOV BX, CX\n";
		code += "MOV AX, " + $1->getAsmName() + "[BX]\n" + "MOV " + temp + ", AX\n" +
		"MOV AX, " + $1->getAsmName() + "[BX]\nINC AX\n" +
		"MOV " + $1->getAsmName() + "[BX], AX\n";
	} else {
		code += "MOV AX, " + $1->getAsmName() + "\nMOV " + temp + ", AX\n" +
		"INC " + $1->getAsmName() + "\n";
	}
	$$->setCode(code);
	$$->setAsmName(temp);
}
	| variable DECOP {
	$$ = new SymbolInfo($1->getName() + "--", "factor", $1->getDataType());
	printLog("factor : variable DECOP", $$->getName());
	string temp = newTemp();
	string code = "";
	if($$->getIsArray()) {
		code += "MOV BX, CX\n";
		code += "MOV AX, " + $1->getAsmName() + "[BX]\n" + "MOV " + temp + ", AX\n" +
		"MOV AX, " + $1->getAsmName() + "[BX]\nDEC AX\n" +
		"MOV " + $1->getAsmName() + "[BX], AX\n";
	} else {
		code+="MOV AX, " + $1->getAsmName() + "\nMOV "+ temp + ",AX\n" +	
		"DEC " + $1->getAsmName() + "\n";
	}
	$$->setCode(code);
	$$->setAsmName(temp);
}
	;
	
argument_list : arguments {
	printLog("argument_list : arguments", $1->getName());
	$$->setCode($1->getCode());	
}
	| {
	$$ = new SymbolInfo("", "argument_list");
	printLog("argument_list : ", $$->getName());
}
	;
	
arguments : arguments COMMA logic_expression {
	argumentTypes.push_back($3->getDataType());
	$$ = new SymbolInfo($1->getName() + "," + $3->getName(), "arguments");
	printLog("arguments : arguments COMMA logic_expression", $$->getName());
	argumentSymbolInfos.push_back($3);
	$$->setCode($1->getCode() + $3->getCode());
		cout << $3->getName() << " " << $3->getDataType() << " yo\n";
}
	      | logic_expression {
	argumentTypes.push_back($1->getDataType());
	printLog("arguments : logic_expression", $1->getName());
	argumentSymbolInfos.push_back($1);
	$$->setCode($1->getCode());
		cout << $$->getName() << " " << $$->getDataType() << " yo\n";
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
	asmFile.open("code.asm");
	
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
	
	peepholeOptimize();
	
	return 0;
}


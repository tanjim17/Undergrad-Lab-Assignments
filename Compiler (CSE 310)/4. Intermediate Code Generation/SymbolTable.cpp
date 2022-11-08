#include <bits/stdc++.h>
using namespace std;

extern ofstream log_;

class SymbolInfo {
    string name, type, dataType, code, asmName; //for functions, dataType refers to return type
    SymbolInfo* next;
    vector<string> paramTypes, paramAsmNames;
    bool isFunction;
    bool isArray; //only for code generation purpose
    vector<SymbolInfo*> children; //children in parse tree

public:
    SymbolInfo(string name, string type, string dataType = "", bool isFunction = 0) {
        this->name = name;
        this->type = type;
        this->dataType = dataType;
        this->isFunction = isFunction;
        next = NULL;
        code = asmName = "";
        isArray = false;
        
    }
    void setName(string name) {this->name = name;}
    string getName() {return name;}
    void setAsmName(string asmName) {this->asmName = asmName;}
    string getAsmName() {return asmName;}
    void setType(string type) {this->type = type;}
    string getType() {return type;}
    void setCode(string code) {this->code = code;}
    string getCode() {return code;}
    void setNext(SymbolInfo* next) {this->next = next;}
    string getDataType() {return dataType;} 
    bool getIsFunction() {return isFunction;}
    void setIsArray(bool isArray) {this->isArray = isArray;}
    bool getIsArray() {return isArray;}
    void setParamTypes(vector<string> paramTypes) {this->paramTypes = paramTypes;}
    vector<string> getParamTypes() {return paramTypes;}
    void setParamAsmNames(vector<string> paramAsmNames) {this->paramAsmNames = paramAsmNames;}
    vector<string> getParamAsmNames() {return paramAsmNames;}
    void addChild(SymbolInfo* symbolInfo) {children.push_back(symbolInfo);}
    vector<SymbolInfo*> getChildren() {return children;}
    SymbolInfo* getNext() {return next;}
};

class ScopeTable {
    int numOfBuckets;
    SymbolInfo** buckets;
    ScopeTable* parent;
    string id;
    int lastChildId; // for tracking scope table id
    int* tokenCount;
    
    int hash_(string key, int numOfBuckets) {
		int sumOfAsciiValues = 0;
		for (int i = 0; i<key.size(); i++)
		    sumOfAsciiValues += key[i];
		return sumOfAsciiValues % numOfBuckets;
	}

public:
    ScopeTable(int numOfBuckets, ScopeTable* parent, int id) {
        this->numOfBuckets = numOfBuckets;
        buckets = new SymbolInfo*[numOfBuckets];
        tokenCount = new int[numOfBuckets]();
        for(int i=0; i<numOfBuckets; i++)
            buckets[i] = NULL;
        this->parent = parent;
        if(parent != NULL)
            this->id = parent->getId() + "_" + to_string(id);
        else
            this->id = to_string(id);
        lastChildId = 0;
    }

    bool insert(SymbolInfo* symbolInfo) {
    	string name = symbolInfo->getName();
        int bucketNo = hash_(name, numOfBuckets);
        SymbolInfo* current = buckets[bucketNo];
        if(current == NULL) {
            buckets[bucketNo] = symbolInfo;
            tokenCount[bucketNo] ++;
            return true;
        }
        if(current->getName() == name) {
            //fout << name + " already exists in current ScopeTable\n\n";
            return false;
        }
        int index = 1;
        while(current->getNext() != NULL) {
            if(current->getNext()->getName() == name) {
                //fout << name + " already exists in current ScopeTable\n\n";
                return false;
            }
            current = current->getNext();
            index ++;
        }
        current->setNext(symbolInfo);
        tokenCount[bucketNo] ++;
        return true;
    }

    SymbolInfo* lookUp(string name) {
        int bucketNo = hash_(name, numOfBuckets);
        SymbolInfo* current = buckets[bucketNo];
        int index = 0;
        while(current != NULL) {
            if(current->getName() == name) {
                //fout << "Found in ScopeTable # " + id + " at position " << bucketNo << ", " << index << endl;
                return current;
            }
            current = current->getNext();
            index ++;
        }
        return NULL;
    }

    bool delete_(string name) {
        int bucketNo = hash_(name, numOfBuckets);
        SymbolInfo* current = buckets[bucketNo];
        if(current == NULL) {
            //fout << "Not Found\n";
            return false;
        }
        if(current->getName() == name) {
            //fout << "Deleted entry " << bucketNo << ", 0 from current ScopeTable\n";
            tokenCount[bucketNo] --;
            buckets[bucketNo] = current->getNext();
            delete current;
            return true;
        }
        int index = 1;
        while(current->getNext() != NULL) {
            if(current->getNext()->getName() == name) {
                //fout << "Deleted entry " << bucketNo << ", " << index << " from current ScopeTable\n";
                tokenCount[bucketNo] --;
                SymbolInfo* temp = current->getNext();
                current->setNext(temp->getNext());
                delete temp;
                return true;
            }
            current = current->getNext();
            index ++;
        }
        //fout << "Not Found" << endl;
        return false;
    }

    void print(ofstream& fout) {
        fout << "ScopeTable # " + id << endl;
        for(int i=0; i<numOfBuckets; i++) {
            if(tokenCount[i] == 0) continue;        
            fout << " " << i << " --> ";
            SymbolInfo* current = buckets[i];
            while(current != NULL) {
                fout << "< " << current->getName() << " : " << current->getType() << "> ";
                current = current->getNext();
            }
            fout << endl;
        }
        fout << endl;
    }

    int increaseAndGetLastChildId() {
        return ++ lastChildId;
    }

    ScopeTable* getParent() {return parent;}
    string getId() {return id;}

    ~ScopeTable() {
        for(int i=0; i<numOfBuckets; i++) {
            SymbolInfo* current = buckets[i];
            while(current != NULL) {
                SymbolInfo* temp = current->getNext();
                delete current;
                current = temp;
            }
        }
        delete[] buckets;
        delete[] tokenCount;
    }
};

class SymbolTable {
    int numOfBuckets;
    ScopeTable* currentScopeTable;
public:
    SymbolTable(int numOfBuckets) {
        this->numOfBuckets = numOfBuckets;
        currentScopeTable = new ScopeTable(numOfBuckets, NULL, 1);
    }

    void enterScope() {
        ScopeTable* newScopeTable = new ScopeTable(numOfBuckets, currentScopeTable, currentScopeTable->increaseAndGetLastChildId());
        log_ << "New ScopeTable with id " + newScopeTable->getId() + " created\n\n";
        currentScopeTable = newScopeTable;
    }

    void exitScope() {
        ScopeTable* temp = currentScopeTable->getParent();
        printAllScopeTables(log_);
        log_ << "ScopeTable with id " << currentScopeTable->getId() << " removed\n\n";
        delete currentScopeTable;
        currentScopeTable = temp;
    }

    bool insert(SymbolInfo* symbolInfo) {
        return currentScopeTable->insert(symbolInfo);
    }

    bool delete_(string name) {
        return currentScopeTable->delete_(name);
    }

    SymbolInfo* lookUp(string name) {
        ScopeTable* current = currentScopeTable;
        while(current != NULL) {
            SymbolInfo* symbolInfo = current->lookUp(name);
            if(symbolInfo != NULL)
                return symbolInfo;
            current = current->getParent();
        }
        //fout << "Not found\n";
        return NULL;
    }

    void printCurrentScopeTable(ofstream& fout) {
        currentScopeTable->print(fout);
    }

    void printAllScopeTables(ofstream& fout) {
        ScopeTable* current = currentScopeTable;
        while(current != NULL) {
            current->print(fout);
            current = current->getParent();
        }
    }
    
    string getCurrentScopeId() {
    	return currentScopeTable->getId();
    }

    ~SymbolTable() {
        while(currentScopeTable != NULL) {
            ScopeTable* temp = currentScopeTable->getParent();
            delete currentScopeTable;
            currentScopeTable = temp;
        }
    }
};

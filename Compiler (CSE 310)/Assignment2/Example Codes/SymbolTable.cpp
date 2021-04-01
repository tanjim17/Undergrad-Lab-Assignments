#include <bits/stdc++.h>
using namespace std;

ifstream fin;
ofstream fout;

int hash_(string key, int numOfBuckets) {
    int sumOfAsciiValues = 0;
    for (int i = 0; i<key.size(); i++)
        sumOfAsciiValues += key[i];
    return sumOfAsciiValues % numOfBuckets;
}

vector<string> stringSplit(string str) {
    istringstream ss(str);
    vector<string> tokenList;
    string token;
    while (ss >> token)
        tokenList.push_back(token);
    return tokenList;
}

class SymbolInfo {
    string name;
    string type;
    SymbolInfo* next;

public:
    SymbolInfo(string name, string type) {
        this->name = name;
        this->type = type;
        next = NULL;
    }
    void setName(string name) {this->name = name;}
    void setType(string type) {this->type = type;}
    void setNext(SymbolInfo* next) {this->next = next;}
    string getName() {return name;}
    string getType() {return type;}
    SymbolInfo* getNext() {return next;}
};

class ScopeTable {
    int numOfBuckets;
    SymbolInfo** buckets;
    ScopeTable* parent;
    string id;
    int lastChildId; // for tracking scope table id

public:
    ScopeTable(int numOfBuckets, ScopeTable* parent, int id) {
        this->numOfBuckets = numOfBuckets;
        buckets = new SymbolInfo*[numOfBuckets];
        for(int i=0; i<numOfBuckets; i++)
            buckets[i] = NULL;
        this->parent = parent;
        if(parent != NULL)
            this->id = parent->getId() + "." + to_string(id);
        else
            this->id = to_string(id);
        lastChildId = 0;
    }

    bool insert(string name, string type) {
        int bucketNo = hash_(name, numOfBuckets);
        SymbolInfo* current = buckets[bucketNo];
        if(current == NULL) {
            buckets[bucketNo] = new SymbolInfo(name, type);
            fout << "Inserted in ScopeTable # " + id + " at position " << bucketNo << ", 0" << endl;
            return true;
        }
        if(current->getName() == name) {
            fout << "<" + name + ", " + current->getType() + "> already exists in current ScopeTable\n";
            return false;
        }
        int index = 1;
        while(current->getNext() != NULL) {
            if(current->getNext()->getName() == name) {
                fout << "<" + name + ", " + current->getNext()->getType() + "> already exists in ScopeTable\n";
                return false;
            }
            current = current->getNext();
            index ++;
        }
        current->setNext(new SymbolInfo(name, type));
        fout << "Inserted in ScopeTable # " + id + " at position " << bucketNo << ", " << index << endl;
        return true;
    }

    SymbolInfo* lookUp(string name) {
        int bucketNo = hash_(name, numOfBuckets);
        SymbolInfo* current = buckets[bucketNo];
        int index = 0;
        while(current != NULL) {
            if(current->getName() == name) {
                fout << "Found in ScopeTable # " + id + " at position " << bucketNo << ", " << index << endl;
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
            fout << "Not Found\n";
            return false;
        }
        if(current->getName() == name) {
            fout << "Deleted entry " << bucketNo << ", 0 from current ScopeTable\n";
            buckets[bucketNo] = current->getNext();
            delete current;
            return true;
        }
        int index = 1;
        while(current->getNext() != NULL) {
            if(current->getNext()->getName() == name) {
                fout << "Deleted entry " << bucketNo << ", " << index << " from current ScopeTable\n";
                SymbolInfo* temp = current->getNext();
                current->setNext(temp->getNext());
                delete temp;
                return true;
            }
            current = current->getNext();
            index ++;
        }
        fout << "Not Found" << endl;
        return false;
    }

    void print(ofstream& fout) {
        fout << "ScopeTable # " + id << endl;
        for(int i=0; i<numOfBuckets; i++) {
            fout << i << " --> ";
            SymbolInfo* current = buckets[i];
            while(current != NULL) {
                fout << "<" << current->getName() << " : " << current->getType() << "> ";
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
        fout << "New ScopeTable with id " + newScopeTable->getId() + " created\n";
        currentScopeTable = newScopeTable;
    }

    void exitScope() {
        ScopeTable* temp = currentScopeTable->getParent();
        fout << "ScopeTable with id " << currentScopeTable->getId() << " removed\n";
        delete currentScopeTable;
        currentScopeTable = temp;
    }

    bool insert(string name, string type) {
        return currentScopeTable->insert(name, type);
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
        fout << "Not found\n";
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

    ~SymbolTable() {
        while(currentScopeTable != NULL) {
            ScopeTable* temp = currentScopeTable->getParent();
            delete currentScopeTable;
            currentScopeTable = temp;
        }
    }
};

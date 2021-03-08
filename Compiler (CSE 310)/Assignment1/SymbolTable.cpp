#include <bits/stdc++.h>
using namespace std;

int NUM_OF_BUCKETS;

int hash_(string key, int numOfBuckets) {
    int sumOfAsciiValues = 0;
    for (int i = 0; i<key.size(); i++)
        sumOfAsciiValues += key[i];
    return sumOfAsciiValues % numOfBuckets;
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
    int level;
    string id;

public:
    ScopeTable(int numOfBuckets, ScopeTable* parent, int level, int id) {
        this->numOfBuckets = numOfBuckets;
        buckets = new SymbolInfo*[numOfBuckets];
        for(int i=0; i<numOfBuckets; i++)
            buckets[i] = NULL;
        this->parent = parent;
        this->level = level;
        if(parent != NULL)
            this->id = parent->getId() + "." + to_string(id);
        else
            this->id = to_string(id);
    }

    bool insert(string name, string type) {
        int bucketNo = hash_(name, numOfBuckets);
        SymbolInfo* current = buckets[bucketNo];
        while(current != NULL) {
            if(current->getName() == name)
                return false;
            current = current->getNext();
        }
        current = new SymbolInfo(name, type);
        return true;
    }

    SymbolInfo* lookUp(string name) {
        int bucketNo = hash_(name, numOfBuckets);
        SymbolInfo* current = buckets[bucketNo];
        while(current != NULL) {
            if(current->getName() == name)
                return current;
            current = current->getNext();
        }
        return NULL;
    }

    bool delete_(string name) {
        int bucketNo = hash_(name, numOfBuckets);
        SymbolInfo* current = buckets[bucketNo];
        if(current == NULL)
            return false;
        if(current->getName() == name) {
            buckets[bucketNo] = current->getNext();
            return true;
        }
        while(current->getNext() != NULL) {
            if(current->getNext()->getName() == name) {
                current->setNext(current->getNext()->getNext());
                return true;
            }
            current = current->getNext();
        }
        return false;
    }

    void print() {
        cout << "ScopeTable # " + id << endl;
        for(int i=0; i<numOfBuckets; i++) {
            cout << i << " --> ";
            SymbolInfo* current = buckets[i];
            while(current != NULL) {
                cout << "<" << current->getName() << ", " << current->getType() << "> ";
                current = current->getNext();
            }
            cout << endl;
        }
        cout << endl;
    }

    ScopeTable* getParent() {return parent;}
    int getLevel() {return level;}
    string getId() {return id;}

    ~ScopeTable() {
        delete[] buckets;
    }
};

class SymbolTable {
    ScopeTable* currentScopeTable;
    vector<int> idCounts; // to track scope table ids of levels
public:
    SymbolTable() {
        idCounts = {1}; //level 1 corresponds to index 0
        currentScopeTable = new ScopeTable(NUM_OF_BUCKETS, NULL, 1, 1);
    }

    void enterScope() {
        int newLevel = currentScopeTable->getLevel() + 1;
        if(newLevel > idCounts.size())
            idCounts.push_back(1);
        else
            idCounts[newLevel - 1] ++;
        ScopeTable* newScopeTable = new ScopeTable(NUM_OF_BUCKETS, currentScopeTable, newLevel, idCounts[newLevel - 1]);
        currentScopeTable = newScopeTable;
    }

    void exitScope() {
        ScopeTable* temp = currentScopeTable->getParent();
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
        return NULL;
    }

    void printCurrentScopeTable() {
        currentScopeTable->print();
    }

    void printAllScopeTables() {
        ScopeTable* current = currentScopeTable;
        while(current != NULL) {
            current->print();
        }
    }
};

int main() {
    return 0;
}

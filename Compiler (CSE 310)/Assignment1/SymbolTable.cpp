#include <iostream>
using namespace std;

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
    ScopeTable* parentScope;

public:
    ScopeTable(int numOfBuckets) {
        this->numOfBuckets = numOfBuckets;
        buckets = new SymbolInfo*[numOfBuckets];
        for(int i=0; i<numOfBuckets; i++)
            buckets[i] = NULL;
        parentScope = NULL;
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
            delete current;
            return true;
        }
        while(current->getNext() != NULL) {
            if(current->getNext()->getName() == name) {
                current->setNext(current->getNext()->getNext());
                delete current->getNext();
                return true;
            }
            current = current->getNext();
        }
        return false;
    }
};

class SymbolTable {

};

int main() {
    return 0;
}

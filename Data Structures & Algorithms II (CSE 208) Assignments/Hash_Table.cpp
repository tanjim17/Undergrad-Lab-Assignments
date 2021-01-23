#include<bits/stdc++.h>
using namespace std;

#define NUM_OF_WORDS 10000
#define NUM_OF_SEARCH 1000
#define WORD_LENGTH 7
#define DOUBLE_HASHING 'd'
#define CUSTOM_PROBING 'c'

vector<pair<string, int>> wordGenerator(int num_of_words, int word_length) {
    int count = 0;
    vector<pair<string, int>> data;
    set<string> words;
    string word;
    int prev_size;
    while(count < num_of_words) {
        word.clear();
        for(int i = 0; i < word_length; i++) {
            word += rand() % 26 + 97;
        }
        prev_size = words.size();
        words.insert(word);
        if(words.size() > prev_size) {
            count++;
            data.push_back({word, count});
        }
    }
    return data;
}

vector<pair<string, int>> getSearchItems(vector<pair<string, int>> data, int num_of_search) {
    int count = 0;
    vector<pair<string, int>> searchItems;
    set<string> words;
    string word;
    int prev_size;
    int index;
    while(count < num_of_search) {
        index = rand() % data.size();
        word = data[index].first;
        prev_size = words.size();
        words.insert(word);
        if(words.size() > prev_size) {
            count++;
            searchItems.push_back(data[index]);
        }
    }
    return searchItems;
}

int hash_(string key, int tableSize) { //polynomial rolling hash function
    int p = 31;
    int power_of_p = 1;
    int hash = 0;
    for (char c : key) {
        hash = (hash + (c - 'a' + 1) * power_of_p) % tableSize;
        power_of_p = (power_of_p * p) % tableSize;
    }
    return hash;
}

/*int hash_(string key, int tableSize) { //djb2 function
    int hash = 5381;
    for (char c : key)
        hash = ((hash << 5) + hash + c) % tableSize;
    return hash;
}*/

int auxHash(string key, int tableSize) {
    int hash = 0;
    for (char c : key)
        hash += ((c ^ 1) * 10) % tableSize;
    hash = (hash + 1) % tableSize;
    return hash;
}

int doubleHash(string key, int tableSize, int i) {
    return (hash_(key, tableSize) + i*auxHash(key, tableSize)) % tableSize;
}

int customHash(string key, int tableSize, int i) {
    int c1 = 7;
    int c2 = 3;
    return (hash_(key, tableSize) + c1*i*auxHash(key, tableSize) + c2*i*i) % tableSize;
}

class ChainedHashTable {
    int size;
    vector<pair<string, int>>* table;

public:
    ChainedHashTable(int size) {
        this->size = size;
        table = new vector<pair<string, int>>[size];
    }

    int search(string key) {
        int i = hash_(key, size);
        int j;
        for(j = 0; j < table[i].size(); j++) {
            if(table[i][j].first == key) {
                break;
            }
        }
        return j;
    }

    int insert(pair<string, int> data) {
        string key = data.first;
        int i = hash_(key, size);
        int j;
        for(j = 0; j < table[i].size(); j++) {
            if(table[i][j].first == key) {
                break;
            }
        }
        if(j == table[i].size()) {
            table[i].push_back(data);
        }
        if(j == 0) return 0;
        return 1;
    }

    void delete_(string key) {
        int i = hash_(key, size);
        int j;
        for(j = 0; j < table[i].size(); j++) {
            if(table[i][j].first == key) {
                table[i].erase(table[i].begin() + j);
                break;
            }
        }
    }
};

class HashTable {
    int size;
    pair<string, int>* table;
    char collisionResMethod; //"d" = double hashing, "c" = custom probing

public:
    HashTable(int size, char collisionResMethod) {
        this->size = size;
        table = new pair<string, int>[size];
        this->collisionResMethod = collisionResMethod;
    }

    int search(string key) {
        int j;
        for(j = 0; j < size; j++) {
            int i;
            if(collisionResMethod == DOUBLE_HASHING)
                i = doubleHash(key, size, j);
            else
                i = customHash(key, size, j);
            if(table[i].first == "" || table[i].first == key) {
                break;
            }
        }
        return j;
    }

    int insert(pair<string, int> data) {
        string key = data.first;
        int j;
        for(j = 0; j < size; j++) {
            int i;
            if(collisionResMethod == DOUBLE_HASHING)
                i = doubleHash(key, size, j);
            else
                i = customHash(key, size, j);
            if(table[i].first == "") {
                table[i] = {data};
                break;
            }
            else if(table[i].first == key){
                break;
            }
        }
        return j;
    }

    void delete_(string key) {
        for(int j = 0; j < size; j++) {
            int i;
            if(collisionResMethod == DOUBLE_HASHING)
                i = doubleHash(key, size, j);
            else
                i = customHash(key, size, j);
            if(table[i].first == "") {
                break;
            }
            else if(table[i].first == key){
                table[i].first = "";
                table[i].second = 0;
                break;
            }
        }
    }
};

int main() {
    int tableSize;
    cout << "Enter table size:\n";
    cin >> tableSize;

    ChainedHashTable t1(tableSize);
    HashTable t2(tableSize, DOUBLE_HASHING);
    HashTable t3(tableSize, CUSTOM_PROBING);

    //number of collisions
    int c1 = 0;
    int c2 = 0;
    int c3 = 0;

    //number of probes
    int p1 = 0;
    int p2 = 0;
    int p3 = 0;

    vector<pair<string, int>> data = wordGenerator(NUM_OF_WORDS, WORD_LENGTH);

    //finding no. of collision
    for (auto d : data) {
        c1 += t1.insert(d);
        c2 += t2.insert(d);
        c3 += t3.insert(d);
    }
    cout << "Number of Collisions:\n";
    cout << "Chaining Method: " << c1 << endl;
    cout << "Double Hashing: " << c2 << endl;
    cout << "Custom Probing: " << c3 << endl;

    //finding average probe
    vector<pair<string, int>> searchItems = getSearchItems(data, NUM_OF_SEARCH);
    for (auto d : searchItems) {
        p1 += t1.search(d.first);
        p2 += t2.search(d.first);
        p3 += t3.search(d.first);
    }
    cout << "Average Probes:\n";
    cout << "Chaining Method: " << (double)p1 / NUM_OF_SEARCH << endl;
    cout << "Double Hashing: " << (double)p2 / NUM_OF_SEARCH << endl;
    cout << "Custom Probing: " << (double)p3 / NUM_OF_SEARCH << endl;

    return 0;
}

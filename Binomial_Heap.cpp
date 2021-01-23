#include<bits/stdc++.h>
using namespace std;

ofstream fout;

class Node {
    int key;
    Node* parent;
    vector<Node*> children;
public:
    int level; //for printing purpose

    Node(int key) {
        this->key = key;
        parent = NULL;
    }

    void addChild(Node* node) { children.push_back(node); }

    int getDegree() { return children.size(); }

    int getKey() { return this->key; }

    vector<Node*> getChildren() { return children; }

    Node* getParent() { return this->parent; }

    void setParent(Node* parent) { this->parent = parent; }
};

class BinomialHeap {
    vector<Node*> roots;

    Node* mergeTree(Node* root1, Node* root2) {
        if(root1->getKey() > root2->getKey()) {
            swap(root1, root2);
        }
        root1->addChild(root2);
        root2->setParent(root1);
        return root1;
    }

public:
    void addTree(Node* root) { //used in building heap from children of extracted root
        roots.push_back(root);
    }

    void insert(int key) {
        if(roots.size() == 0) {
            roots.push_back(new Node(key));
        }
        else {
            BinomialHeap h2;
            h2.insert(key);
            this->union_(h2);
        }
    }

    void union_(BinomialHeap h2) {
        vector<Node*> roots1 = this->roots;
        vector<Node*> roots2 = h2.roots;

        //concatenate tree lists of the heaps in ascending order
        stack<Node*> allRoots;
        int i = roots1.size() - 1;
        int j = roots2.size() - 1;
        while (i >= 0 && j >= 0) {
            if(roots1[i]->getDegree() > roots2[j]->getDegree()) {
                allRoots.push(roots1[i]);
                i--;
            }
            else {
                allRoots.push(roots2[j]);
                j--;
            }
        }
        while (i >= 0) {
            allRoots.push(roots1[i]);
            i--;
        }
        while (j >= 0) {
            allRoots.push(roots2[j]);
            j--;
        }

        //merge trees having same degree
        this->roots.clear();
        while(allRoots.size() > 1) {
            Node* root1 = allRoots.top(); allRoots.pop();
            Node* root2 = allRoots.top(); allRoots.pop();
            if(root1->getDegree() == root2->getDegree()) {
                Node* mergedTreeRoot = mergeTree(root1, root2);
                allRoots.push(mergedTreeRoot);
            }
            else if(root1->getDegree() > root2->getDegree()) {
                this->roots.push_back(root2);
                allRoots.push(root1);
            }
            else {
                this->roots.push_back(root1);
                allRoots.push(root2);
            }
        }
        this->roots.push_back(allRoots.top());
    }

    int findMin() {
        int minimum = INFINITY;
        for(auto root: this->roots) {
            minimum = min(minimum, root->getKey());
        }
        return minimum;
    }

    int extractMin() {
        if(roots.size() == 0) return INFINITY;
        int minIndex = 0;
        for(int i = 0; i < roots.size(); i++) {
            if(roots[i]->getKey() < roots[minIndex]->getKey()) {
                minIndex = i;
            }
        }
        int minimum = roots[minIndex]->getKey();

        //create a heap using the children of extracted root and union with it
        BinomialHeap h2;
        vector<Node*> children = roots[minIndex]->getChildren();
        for(auto child: children) {
            h2.addTree(child);
        }
        delete roots[minIndex];
        this->roots.erase(this->roots.begin() + minIndex);
        if(children.size() > 0) {
            this->union_(h2);
        }
        return minimum;
    }

    void print() {
        if(this->roots.size() == 0) {
            cout << "Heap is empty!\n"; fout << "Heap is empty!\n";
            return;
        }
        cout << "Printing Binomial Heap...\n"; fout << "Printing Binomial Heap...\n";
        for(auto root: this->roots) {
            cout << "Binomial Tree, B" << root->getDegree(); fout << "Binomial Tree, B" << root->getDegree();
            queue<Node*> q;
            q.push(root);
            int currentLevel = -1;
            while(!q.empty()) {
                Node* node = q.front(); q.pop();
                if(node != root) {
                    node->level = node->getParent()->level + 1;
                }
                else {
                    node->level = 0;
                }
                if(currentLevel < node->level) {
                    currentLevel = node->level;
                    cout << "\nLevel " << currentLevel << " : "; fout << "\nLevel " << currentLevel << " : ";
                }
                cout << node->getKey() << " "; fout << node->getKey() << " ";

                vector<Node*> children = node->getChildren();
                for(int i = children.size() - 1; i >= 0; i--) {
                    q.push(children[i]);
                }
            }
            cout << endl; fout << endl;
        }
    }
};

int main() {
    ifstream fin;
    fin.open("input.txt");
    fout.open("output.txt");

    BinomialHeap h;

    char operation;
    string line;
    while(getline(fin, line)) {
        istringstream sin(line);
        sin >> operation;
        if(operation == 'I') {
            int key;
            sin >> key;
            h.insert(key);
        }
        else if(operation == 'U') {
            BinomialHeap h2;
            int key;
            while(sin >> key) {
                h2.insert(key);
            }
            h.union_(h2);
        }
        else if(operation == 'F') {
            int minimum = h.findMin();
            cout << "Find-Min returned " << minimum << endl; fout << "Find-Min returned " << minimum << endl;
        }
        else if(operation == 'E') {
            int minimum = h.extractMin();
            cout << "Extract-Min returned " << minimum << endl; fout << "Extract-Min returned " << minimum << endl;
        }
        else if(operation == 'P') {
            h.print();
        }
        else break;
    }
    fin.close();
    fout.close();
    return 0;
}

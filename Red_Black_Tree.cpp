#include<bits/stdc++.h>
using namespace std;

class Node {
public:
    int key;
    char color; //black: "b" and red: "r"
    Node* parent;
    Node* left;
    Node* right;
    Node(int key, char color, Node* parent, Node* left, Node* right) {
        this->key = key;
        this->color = color;
        this->parent = parent;
        this->left = left;
        this->right = right;
    }
    Node() { //for nil node
        this->color = 'b';
    }
};

class RedBlackTree {
    Node* root;
    Node* nil;

    void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if(y->left != nil) {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if(x->parent == nil) {
            root = y;
        }
        else if(x == x->parent->left) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    void rightRotate(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        if(x->right != nil) {
            x->right->parent = y;
        }
        x->parent = y->parent;
        if(y->parent == nil) {
            root = x;
        }
        else if(y == y->parent->left) {
            y->parent->left = x;
        }
        else {
            y->parent->right = x;
        }
        x->right = y;
        y->parent = x;
    }

    void transplant(Node* u, Node* v) {
        if(u->parent == nil) {
            root = v;
        }
        else if(u == u->parent->left) {
            u->parent->left = v;
        }
        else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    Node* treeMinimum(Node* x) {
        while(x->left != nil) {
            x = x->left;
        }
        return x;
    }

    void insertFixUp(Node* z) {
        while(z->parent->color == 'r') {
            if(z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right;
                if(y->color == 'r') {
                    z->parent->color = 'b';
                    y->color = 'b';
                    z->parent->parent->color = 'r';
                    z = z->parent->parent;
                }
                else {
                    if(z == z->parent->right) {
                        z = z->parent;
                        leftRotate(z);
                    }
                    z->parent->color = 'b';
                    z->parent->parent->color = 'r';
                    rightRotate(z->parent->parent);
                }
            }
            else {
                Node* y = z->parent->parent->left;
                if(y->color == 'r') {
                    z->parent->color = 'b';
                    y->color = 'b';
                    z->parent->parent->color = 'r';
                    z = z->parent->parent;
                }
                else {
                    if(z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->color = 'b';
                    z->parent->parent->color = 'r';
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = 'b';
    }

    void deleteFixUp(Node* x) {
        Node* w;
        while(x != root && x->color == 'b') {
            if(x == x->parent->left) {
                w = x->parent->right;
                if(w->color == 'r') {
                    w->color = 'b';
                    x->parent->color = 'r';
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                if(w->left->color == 'b' && w->right->color == 'b') {
                    w->color = 'r';
                    x = x->parent;
                }
                else {
                    if(w->right->color == 'b') {
                        w->left->color = 'b';
                        w->color = 'r';
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = 'b';
                    w->right->color = 'b';
                    leftRotate(x->parent);
                    x = root;
                }
            }
            else {
                w = x->parent->left;
                if(w->color == 'r') {
                    w->color = 'b';
                    x->parent->color = 'r';
                    rightRotate(x->parent);
                    w = x->parent->left;
                }
                if(w->left->color == 'b' && w->right->color == 'b') {
                    w->color = 'r';
                    x = x->parent;
                }
                else {
                    if(w->left->color == 'b') {
                        w->right->color = 'b';
                        w->color = 'r';
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = 'b';
                    w->left->color = 'b';
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = 'b';
    }

    void printPreOrder(Node* node) {
        if(node == nil) return;
        cout << node->key << ":" << node->color;
        if(node->left == nil && node->right == nil) return; //double empty brackets won't be printed when both children are nil
        cout << "(";
        printPreOrder(node->left);
        cout << ")(";
        printPreOrder(node->right);
        cout << ")";
    }

public:
    RedBlackTree() {
        nil = new Node();
        root = nil;
    }

    void insert(int key) {
        Node* y = nil;
        Node* x = root;
        while(x != nil) {
            y = x;
            if(key < x->key) x = x->left;
            else x = x->right;
        }

        Node* newNode = new Node(key, 'r', y, nil, nil);
        if(y == nil) {
            root = newNode;
        }
        else if(newNode->key < y->key) {
            y->left = newNode;
        }
        else {
            y->right = newNode;
        }

        insertFixUp(newNode);
    }

    void delete_(int key) {
        Node* z = findNode(key);
        if(z == nil) return;
        Node* y = z;
        int yOriginalColor = y->color;

        Node* x;
        if(z->left == nil) {
            x = z->right;
            transplant(z, z->right);
        }
        else if(z->right == nil) {
            x = z->left;
            transplant(z, z->left);
        }
        else {
            y = treeMinimum(z->right);
            yOriginalColor = y->color;
            x = y->right;
            if(y->parent == z) {
                x->parent = y;
            }
            else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        delete z;

        if(yOriginalColor == 'b') {
            deleteFixUp(x);
        }
    }

    bool find(int key) {
        Node* currentNode = root;
        while(currentNode != nil) {
            if(key == currentNode->key) return true;
            else if(key < currentNode->key) currentNode = currentNode->left;
            else currentNode = currentNode->right;
        }
        return false;
    }

    Node* findNode(int key) {
        Node* currentNode = root;
        while(currentNode != nil) {
            if(key == currentNode->key) return currentNode;
            else if(key < currentNode->key) currentNode = currentNode->left;
            else currentNode = currentNode->right;
        }
        return currentNode;
    }

    void printTree() {
        printPreOrder(root);
        cout<<"\n";
    }
};

int main() {
    ifstream fin;
    fin.open("input.txt");

    RedBlackTree redBlackTree;

    char operation; // "F": find, "I": insert, "D": delete
    int key;
    while(!fin.eof()) {
        fin >> operation >> key;
        if(operation == 'F') {
            if(redBlackTree.find(key)) cout << "True\n";
            else cout << "False\n";
        }
        else if(operation == 'I'){
            redBlackTree.insert(key);
            redBlackTree.printTree();
        }
        else if(operation == 'D'){
            redBlackTree.delete_(key);
            redBlackTree.printTree();
        }
        else break;
    }
    return 0;
}

#include <iostream>
using namespace std;

class Disjoint_Sets
{
    int *parent;
    int *rank;
    int size;

public:
    Disjoint_Sets(int n)
    {
        size = n;
        parent = new int[n];
        rank = new int[n];

        for(int i = 0; i < n; i++){
            parent[i] = -1;
            rank[i] = -1;
        }
    }

    void make_set(int x)
    {
        if(parent[x] == -1){
            parent[x] = x;
            rank[x] = 0;
        }

        else
            cout << "input already exists in some set\n";
    }

    int find_set(int x)
    {
        if(parent[x] != x && parent[x] != -1)
            parent[x] = find_set(parent[x]);

        return parent[x];
    }

    void union_(int x, int y)
    {
        if(parent[x] == -1)
            cout << "first input doesn't exist in any set\n";

        else if(parent[y] == -1)
            cout << "second input doesn't exist in any set\n";

        else{
            x = find_set(x);
            y = find_set(y);

            if(x == y)
                cout << "already in same set\n";

            else{
                if(rank[x] < rank[y])
                    parent[x] = y;

                else if(rank[x] > rank[y])
                    parent[y] = x;

                else{
                    parent[y] = x;
                    rank[x] ++;
                }
            }
        }
    }

    void print(int x)
    {

        if(parent[x] == -1){
            cout << "input doesn't exist in any set\n";
        }

        else if(parent[x] != x){
            cout << "input is not representative of any set\n";
        }

        else{
            cout << "{";

            for(int i = 0; i < size; i++){

                if(find_set(i) == x)
                    cout << i << ", ";
            }
            cout << "\b\b}\n";
        }
    }
};

int main()
{
    int n, i, x, y;

    cout << "Enter capacity disjoint sets: ";
    cin >> n;

    Disjoint_Sets d(n);

    while(true){

        cout << endl;
        cout << "1. Make_Set\n";
        cout << "2. Find_Set\n";
        cout << "3. Union\n";
        cout << "4. Print\n";
        cout << "5. Exit\n";
        cout << endl;

        cin >> i;

        switch(i){

        case 1:
            cin >> x;

            if(x >= n){
                cout << "input is not within capacity of disjoint set.\n";
                break;
            }

            d.make_set(x);
            break;

        case 2:
            cin >> x;

            if(x >= n){
                cout << "input is not within capacity of disjoint set.\n";
                break;
            }

            y = d.find_set(x);

            if(y == -1)
                cout << "input doesn't exist in any set\n";
            else
                cout << y;
            break;

        case 3:
            cin >> x >> y;

            if(x >= n || y >= n){
                cout << "input is not within capacity of disjoint set.\n";
                break;
            }

            d.union_(x, y);
            break;

        case 4:
            cin >> x;

            if(x >= n){
                cout << "input is not within capacity of disjoint set.\n";
                break;
            }

            d.print(x);
            break;

        case 5:
            exit(0);
        }
    }
}
























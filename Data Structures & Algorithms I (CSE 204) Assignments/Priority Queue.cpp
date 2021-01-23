#include <iostream>
using namespace std;

class Priority_Queue
{
    int* a;
    int capacity;
    int size;

public:

    Priority_Queue(int capacity)
    {
        this->capacity = capacity;
        a = new int[capacity + 1];
        size = 0;
    }

    void insert(int n)
    {
        if(size == capacity){
            cout << "Overflow\n";
            return;
        }

        size++;
        increaseKey(size, n);
    }

    int findMax()
    {
        return a[1];
    }

    int extractMax()
    {
        if(size == 0){
            cout << "Underflow" << endl;
            return -1;
        }

        int max = a[1];
        int temp = a[size];

        size--;
        decreaseKey(1, temp);

        return max;
    }

    void increaseKey(int i, int n)
    {
        a[i] = n;

        while(i > 1){
            if(a[i] > a[i / 2]){
                swap(a[i], a[i / 2]);
                i /= 2;
            }

            else break;
        }
    }

    void decreaseKey(int i, int n)
    {
        a[i] = n;

        int j = 2 * i;

        while(j <= size){

            if(j != size && a[j + 1] > a[j])
                j++;

            if(a[j] > a[i]){
                swap(a[j], a[i]);
                i = j;
                j = 2 * i;
            }

            else break;
        }

    }

    void print()
    {
        for(int i = 1; i <= size; i++)
            cout << a[i] << ' ';
        cout << endl;
    }
};

int main()
{
    int n, i, x, y;

    cout << "Enter capacity of heap: ";
    cin >> n;

    Priority_Queue q(n);

    while(true)
    {
        cout << endl;
        cout << "1. Insert\n";
        cout << "2. Find Max\n";
        cout << "3. Extract Max\n";
        cout << "4. Increase Key\n";
        cout << "5. Decrease Key\n";
        cout << "6. Print\n";
        cout << "7. Exit\n";
        cout << endl;

        cin >> i;

        switch(i)
        {
        case 1:
            cout << "Enter number to be inserted: ";
            cin >> x;
            q.insert(x);
            break;

        case 2:
            cout << "Maximum of the heap is: " << q.findMax() << endl;
            break;

        case 3:
            cout << "Maximum of the heap is extracted, which was: " << q.extractMax() << endl;
            break;

        case 4:
            cout << "Enter index and new key: ";
            cin >> x >> y;
            q.increaseKey(x, y);
            break;

        case 5:
            cout << "Enter index and new key: ";
            cin >> x >> y;
            q.decreaseKey(x, y);
            break;

        case 6:
            q.print();
            break;

        case 7:
            exit(0);
        }
    }
}
























#include <iostream>
#include <cmath>
#include <ctime>
#include <algorithm>
using namespace std;

char** get_power_set(char a[], int len, int power_set_size)
{
    char** power_set = new char*[power_set_size];

    for(int i = 0; i < power_set_size; i++){

        int set_size = 1;
        power_set[i] = new char[set_size];
        int operand = 1;

        for(int j = 0; j < len; j++, operand <<= 1){

            if(i & operand){

                power_set[i][set_size - 1] = a[j];
                set_size++;
                power_set[i] = (char*)realloc(power_set[i], set_size);
            }
        }
        power_set[i][set_size - 1] = 0;
    }
    return power_set;
}

int main()
{
    struct timespec start, stop;
    double elapsed;
    int n;
    cout << "Enter set size: ";
    cin >> n;
    char a[n];

    char alphanum []=
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

     for(int i = 0; i < n; i++){

        char x;

        do{
            x = alphanum[rand() % (sizeof(alphanum) - 1)];
        }
        while(find(a, a+i, x) != a+i);

        a[i] = x;
    }

    int len = sizeof(a);
    int power_set_size = (int)pow(2, len);

    clock_gettime(CLOCK_MONOTONIC, &start);

    char **power_set = get_power_set(a, len, power_set_size);

    clock_gettime(CLOCK_MONOTONIC, &stop);

    elapsed = (stop.tv_sec - start.tv_sec) * 1e3;
    elapsed = elapsed + (stop.tv_nsec - start.tv_nsec) / 1e6;
    cout << "running time: " << elapsed << " millisec" << endl;


    //print the power set
    /*for(int i = 0; i < power_set_size; i++){

        cout << "{ ";
        for(int j = 0; power_set[i][j]; j++){

            cout << power_set[i][j] << ' ';
        }
        cout << '}' << endl;
    }*/

}

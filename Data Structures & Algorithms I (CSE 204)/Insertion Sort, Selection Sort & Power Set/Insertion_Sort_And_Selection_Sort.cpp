#include<iostream>
#include<ctime>
using namespace std;

void insertion_sort(int a[], int len)
{
    for(int i = 1; i < len; i++){

        int key = a[i];

        for(int j = i - 1; j > -1; j--){

            if(key < a[j])
                a[j+1] = a[j];
            else{
                a[j+1] = key;
                break;
            }
        }
    }
}

void selection_sort(int a[], int len)
{
    for(int i = 0; i < len - 1; i++){

        int index_min = i;

        for(int j = i + 1; j < len; j++){

            if(a[j] < a[index_min])
                index_min = j;
        }

        if(index_min != i){
            int temp = a[i];
            a[i] = a[index_min];
            a[index_min] = temp;
        }
    }
}

double calculate_time(void (*f) (int [], int), int a[], int len)
{
    struct timespec start, stop;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);

    (*f)(a, len);

    clock_gettime(CLOCK_MONOTONIC, &stop);

    elapsed = (stop.tv_sec - start.tv_sec) * 1e3;
    elapsed = elapsed + (stop.tv_nsec - start.tv_nsec) / 1e6;

    return elapsed;
}
int main()
{
    double insert_ave, insert_best, insert_worst, selection_ave, selection_best, selection_worst;
    int n;

    cout << "Enter size of array: ";
    cin >> n;
    cout << endl;
    int a[n], b[n];

    for(int i = 0; i < n; i++){
        a[i] = rand();
        b[i] = a[i];
    }

    insert_ave = calculate_time(insertion_sort, a, n);
    selection_ave = calculate_time(selection_sort, b, n);

    insert_best = calculate_time(insertion_sort, a, n);
    selection_best = calculate_time(selection_sort, a, n);

    for(int i = 0; i < n; i++){
        b[n-i-1] = a[i];
    }
    insert_worst = calculate_time(insertion_sort, b, n);

    for(int i = 0; i < n; i++){
        b[n-i-1] = a[i];
    }
    selection_worst = calculate_time(selection_sort, b, n);

    cout << "best case:\n";
    cout << "insertion: " << insert_best << " millisec" << endl << "selection: " << selection_best << " millisec" << endl << endl;

    cout << "average case:\n";
    cout << "insertion: " << insert_ave << " millisec" << endl << "selection: " << selection_ave << " millisec" << endl << endl;

    cout << "worst case:\n";
    cout << "insertion: " << insert_worst << " millisec" << endl << "selection: " << selection_worst << " millisec";
}

#include <cstdio>
#include <iostream>
#include <ctime>
#include <algorithm>

using namespace std;

void merge(int *a, int l, int m, int r)
{
    int n1 = m - l + 1;
    int n2 = r - m;
    int *L = new int[n1];
    int *R = new int[n2];

    for(int i = 0; i < n1; i++)
        L[i] = a[l + i];
    for(int i = 0; i < n2; i++)
        R[i] = a[m + 1 + i];

    int i = 0, j = 0, k = l;

    while(i < n1 && j < n2){
        if(L[i] < R[j]){
            a[k] = L[i];
            i++;
        }
        else{
            a[k] = R[j];
            j++;
        }
        k++;
    }

    while(i < n1){
        a[k] = L[i];
        i++;
        k++;
    }
    while(j < n2){
        a[k] = R[j];
        j++;
        k++;
    }

    delete[] L;
    delete[] R;
}

int partition(int *a, int l, int r)
{
    int pivot = a[r];
    int i = l;

    for (int j = l; j < r; j++){
        if (a[j] <= pivot){
            swap(a[i], a[j]);
            i++;
        }
    }
    swap(a[i], a[r]);

    return i;
}
void mergeSort(int *a, int l, int r)
{
    if(l < r){
        int m = (l + r) / 2;

        mergeSort(a, l, m);
        mergeSort(a, m + 1, r);

        merge(a, l, m, r);
    }
}

void quickSort(int *a, int l, int r)
{
    if(l < r){
        int p = partition(a, l, r);

        quickSort(a, l, p - 1);
        quickSort(a, p + 1, r);
    }
}

int main()
{
    int choice, n, *a;
    struct timespec start, stop;
    double elapsed;

    while(1)
    {
        printf("1. Generate average case\n");
        printf("2. Generate best case\n");
        printf("3. Generate worst case\n");
        printf("4. Apply Merge sort\n");
        printf("5. Apply Quicksort\n");
        printf("6. Print array\n");
        printf("> ");
        scanf("%d", &choice);

        switch(choice)
        {
        case 1:
            printf("Number of elements: ");
            scanf("%d", &n);

            a = new int[n];

            for(int i = 0; i < n; i++)
                a[i] = rand()%100;

            break;

        case 2:
            printf("Number of elements: ");
            scanf("%d", &n);

            a = new int[n];

            for(int i = 0; i < n; i++)
                a[i] = rand()%100;

            sort(a, a + n);
            break;

        case 3:
            printf("Number of elements: ");
            scanf("%d", &n);

            a = new int[n];

            for(int i = 0; i < n; i++)
                a[i] = rand()%100;

            sort(a, a + n, greater<int>());
            break;

        case 4:
            printf("Applying merge sort\n");

            clock_gettime(CLOCK_MONOTONIC, &start);
            mergeSort(a, 0, n - 1);
            clock_gettime(CLOCK_MONOTONIC, &stop);

            elapsed = (stop.tv_sec - start.tv_sec) * 1e3;
            elapsed = elapsed + (stop.tv_nsec - start.tv_nsec) / 1e6;

            cout << "Time taken to finish:\n" << elapsed << " millisec\n";
            break;

        case 5:
            printf("Applying quicksort\n");

            clock_gettime(CLOCK_MONOTONIC, &start);
            quickSort(a, 0, n - 1);
            clock_gettime(CLOCK_MONOTONIC, &stop);

            elapsed = (stop.tv_sec - start.tv_sec) * 1e3;
            elapsed = elapsed + (stop.tv_nsec - start.tv_nsec) / 1e6;

            cout << "Time taken to finish:\n" << elapsed << " millisec\n";
            break;

        case 6:
            printf("Array\n");
            for(int i = 0; i < n; i++)
                cout << a[i] << " ";
            cout << endl;
            break;
        }
    }
}


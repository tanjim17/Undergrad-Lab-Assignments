#include <iostream>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main()
{
	int n;
	cin >> n;
	int a[n];
	int key[5];

	for (int i = 0; i < n; i++) {
		a[i] = rand();
		cout << a[i] << " ";
	}
	cout << endl;

    sort(a,a+n);

	for (int i = 0; i < 5; i++) {
		key[i] = rand();
		cout << key[i] << " ";
	}
	cout << endl;

	double time_for_linear = 0;
	double time_for_binary = 0;

	for (int i = 0; i < 5; i++) {

        /*linear search*/
        auto start = high_resolution_clock::now();

		int j;

		for (j = 0; j < n; j++) {
			if (key[i] == a[j]){
                cout << key[i] << " is found (using linear search)" << endl;
                break;
			}
		}

		if (j == n) {
			cout << key[i] << " is not found (using linear search)" << endl;
		}

        auto stop = high_resolution_clock::now();
		auto elapsed = duration_cast<nanoseconds>(stop - start);
		time_for_linear += elapsed.count();

		/*binary search*/
		start = high_resolution_clock::now();

		int l = 0;
		int r = n - 1;
		int m;

		while (l <= r) {

			m = (l + r) / 2;

			if (key[i] == a[m]) {
				cout << key[i] << " is found (using binary search)" << endl;
				break;
			}

			else if (key[i] < a[m])
				r = m - 1;

			else
				l = m + 1;
		}

		if(l > r)
			cout << key[i] << " is not found (using binary search)" << endl;

		stop = high_resolution_clock::now();
		elapsed = duration_cast<nanoseconds>(stop - start);
		time_for_binary += elapsed.count();

	}

	cout << time_for_linear / 5.0 << " is the average time needed for linear search" << endl;
	cout << time_for_binary / 5.0 << " is the average time needed for binary search" << endl;

}


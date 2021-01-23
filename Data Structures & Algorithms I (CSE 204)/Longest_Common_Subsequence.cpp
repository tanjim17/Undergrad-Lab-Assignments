#include <iostream>
#include <string>
#include <algorithm>

#define diagonal 0
#define up 1
#define left 2

using namespace std;

void print_LCS(int **b, string x, int i, int j)
{
    if(!i || !j)
        return;

    if(b[i][j] == diagonal){

        print_LCS(b, x, i-1, j-1);
        cout << x[i - 1];
    }

    else if(b[i][j] == up) print_LCS(b, x, i-1, j);

    else print_LCS(b, x, i, j-1);
}

void find_LCS(string x, string y)
{
    int m = x.length(), n = y.length();

    int **b = new int*[m + 1], **c = new int*[m + 1];

    for(int i = 0; i <= m; i++)
        b[i] = new int[n + 1], c[i] = new int[n + 1];

    for(int i = 1; i <= m; i++){
        for(int j = 1; j <= n; j++){

            if(x[i - 1] == y[j - 1]){

                c[i][j] = c[i-1][j-1] + 1;
                b[i][j] = diagonal;
            }

            else{
                if(c[i-1][j] >= c[i][j-1]){

                    c[i][j] = c[i-1][j];
                    b[i][j] = up;
                }
                else{
                    c[i][j] = c[i][j-1];
                    b[i][j] = left;
                }
            }
        }
    }

    print_LCS(b, x, m, n);

    delete[] b;
    delete[] c;
}

int main()
{
    string x, y;

    cout << "Enter two sequences:\n";
    cin >> x >> y;

    find_LCS(x, y);

    return 0;
}
























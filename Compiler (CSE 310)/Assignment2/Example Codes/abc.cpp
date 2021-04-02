#include <bits/stdc++.h>
using namespace std;

int main() {
	FILE *fp;
    fp = fopen("input.txt", "r");
      
    // Moving pointer to end
    fseek(fp, -1, SEEK_END);
      
    // Printing position of pointer
    printf("%d", fgetc(fp));
  
    return 0;
}

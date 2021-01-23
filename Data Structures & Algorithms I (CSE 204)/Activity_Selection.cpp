#include <iostream>
#include <algorithm>
using namespace std;

class Activity
{
public:
    int start;
    int end;

    bool operator<(Activity a) {return end < a.end;}
};

void find_max_activities(Activity *ac_list, int n)
{
    int i = 0;
    cout << i << " ";

    for(int j = 1; j < n; j++){

        if(ac_list[j].start >= ac_list[i].end){

            i = j;
            cout << i << " ";
        }
    }
    cout << endl;
}

int main()
{
    int n;

    cout << "Enter number of activities:";
    cin >> n;

    Activity *ac_list = new Activity[n];

    for(int i = 0; i < n; i++){
        ac_list[i].start = rand() % 50;
        ac_list[i].end = (ac_list[i].start+1) + rand() % (50-ac_list[i].start);
    }

    sort(ac_list, ac_list+n);

    for(int i = 0; i < n; i++){
        cout << ac_list[i].start << " " << ac_list[i].end << endl;
    }
    cout << endl;

    find_max_activities(ac_list, n);

}
























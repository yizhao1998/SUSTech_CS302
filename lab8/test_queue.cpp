#include <cstdio>
#include <queue>
#include <list>

using namespace std;

list<int> ls;

int main(){
    ls.push_back(2);
    ls.push_back(3);
    list<int>::iterator iter = --ls.end();
    printf("%d\n", iter);
    ls.pop_front();
    printf("%d\n", iter);
    ls.push_back(9);
    printf("%d\n", iter);
    ls.erase(iter);
    for(list<int>::iterator it = ls.begin(); it != ls.end(); it++){
        printf("%d", *it);
    }
    
    return 0;
}
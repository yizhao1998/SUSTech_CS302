#include <cstdio>
#include <iostream>
#include <map>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>
#define maxr 105

using namespace std;
int remain[maxr];
map<int, int[maxr]> needed;
map<int, int[maxr]> allocated;
map<int, int[maxr]> _max;
int r;
string str;
int pid;
int resource[maxr];
int req[maxr];

void recycle(int pid){
    for(int i=0;i<r;i++){
        remain[i] += _max[pid][i];
    }
    _max.erase(pid);
    allocated.erase(pid);
    needed.erase(pid);
}

bool safe_check(map<int, int[maxr]> &tmp_allocated, map<int, int[maxr]> &tmp_needed){
    map<int, bool> finish;
    int work[maxr];
    for(int i=0;i<r;i++){
        work[i] = remain[i] - req[i];
    }
    for(auto iter = tmp_needed.begin(); iter != tmp_needed.end(); iter++){
        finish[iter->first] = false;
    }
    bool flag_outer;
    for(int i=0;i<tmp_needed.size();i++){
        flag_outer = false;
        for(auto iter = tmp_needed.begin(); iter != tmp_needed.end(); iter++){
            if(!finish[iter->first]){
                bool flag = true;
                for(int i=0;i<r;i++){
                    if(iter->second[i] <= work[i]){
                        flag = false;
                    }
                }
                if(flag){
                    flag_outer = true;
                    for(int i=0;i<r;i++){
                        work[i] += tmp_allocated[iter->first][i];
                    }
                    finish[iter->first] = true;
                }
            }    
        }
        if(!flag_outer){
            break;
        }
    }
    if(flag_outer){
        for(int i=0;i<r;i++){
            remain[i] = work[i];
        }
        for(auto iter = tmp_needed.begin(); iter != tmp_needed.end(); iter++){
            for(int i=0;i<r;i++){
                allocated[iter->first][i] = tmp_allocated[iter->first][i];
                needed[iter->first][i] = tmp_needed[iter->first][i];
            }
        }
        return true;
    }
    else{
        return false;
    }
    
}

bool request_judge(int pid){
    bool flag = false;
    for(int i=0;i<r;i++){
        if(_max[pid][i] < req[i] + needed[pid][i] || req[i] > remain[i]){
            return false;
        }
    }
    map<int, int[maxr]> tmp_needed;
    map<int, int[maxr]> tmp_allocated;
    for(auto iter = needed.begin(); iter != needed.end(); iter++){
        for(int i=0;i<r;i++){
            if(iter->first == pid){
                tmp_needed[iter->first][i] = needed[iter->first][i] - req[i];
                tmp_allocated[iter->first][i] = needed[iter->first][i] + req[i];
            }else{
                tmp_needed[iter->first][i] = needed[iter->first][i];
                tmp_allocated[iter->first][i] = allocated[iter->first][i];
            }
        }
    }
    bool res = safe_check(tmp_allocated, tmp_needed);
    return res;
}

void request_process(int pid){
    for(int i=0;i<r;i++){
        needed[pid][i] -= 0;
        allocated[pid][i] += req[i];
    }
}

bool new_judge(int pid){
    if(_max.find(pid) == _max.end()){
        return false;
    }
    for(int i=0;i<r;i++){
        if(req[i] > resource[i]){
            return false;
        }
    }
    return true;
}

void new_process(int pid){
    for(int i=0;i<r;i++){
        _max[pid][i] = req[i];
        needed[pid][i] = req[i];
        allocated[pid][i] = 0;
    }
}

int main(){
    scanf("%d", &r);
    for(int i=0;i<r;i++){
        cin >> remain[i];
        resource[i] = remain[i];
    }
    bool res;
    while(cin >> pid >> str){
        if(str != "terminate"){
            for(int i=0;i<r;i++){
                cin >> req[i];
            }
            if(str == "request"){
                res = request_judge(pid);
                if(res){
                    cout << "OK" << endl;
                }else{
                    cout << "not OK" << endl;
                }
            }else if(str == "new"){
                res = new_judge(pid);
                if(res){
                    cout << "not OK" << endl;
                }else{
                    new_process(pid);
                }
            }
        }else{
            if(_max.find(pid) != _max.end()){
                recycle(pid);
                cout << "OK" << endl;
            }else{
                cout << "not OK" << endl;
            }
        }
    }
    return 0;
}
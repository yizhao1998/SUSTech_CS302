#include <iostream>
#include <map>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <vector>

typedef unsigned long ul;
//#define DEBUG

using namespace std;
vector<int> remain;
map<int, vector<int>> needed;
map<int, vector<int>> allocated;
map<int, vector<int>> _max;
int r;
string str;
int pid;
vector<int> resource;
vector<int> req;

void recycle(int pid) {
    for (int i = 0; i < r; i++) {
        remain[i] += allocated[pid][i];
    }
    _max.erase(pid);
    allocated.erase(pid);
    needed.erase(pid);
}

bool safe_check(map<int, vector<int>> &tmp_allocated, map<int, vector<int>> &tmp_needed) {
    map<int, bool> finish;
    vector<int> work((ul) r);
    for (int i = 0; i < r; i++) {
        work[i] = remain[i] - req[i];
    }
    for (auto &iter : tmp_needed) {
        finish[iter.first] = false;
    }
    bool flag_outer;
    for (int i = 0; i < tmp_needed.size(); i++) {
        flag_outer = false;
        for (auto &iter : tmp_needed) {
            if (!finish[iter.first]) {
                bool flag = true;
                for (int j = 0; j < r; j++) {
                    if (iter.second[j] > work[j]) {
                        flag = false;
                        break;
                    }
                }
                if (flag) {
                    flag_outer = true;
                    for (int j = 0; j < r; j++) {
                        // imitate recycle resource
                        work[j] += tmp_allocated[iter.first][j];
                    }
                    finish[iter.first] = true;
                }
            }
        }
        if (!flag_outer) {
            break;
        }
    }
    flag_outer = true;
    for (auto &iter : finish) {
        flag_outer &= iter.second;
    }
    if (flag_outer) {
        for (int i = 0; i < r; i++) {
            remain[i] = remain[i] - req[i];
        }
        for (auto iter = tmp_needed.begin(); iter != tmp_needed.end(); iter++) {
            for (int i = 0; i < r; i++) {
                allocated[iter->first][i] = tmp_allocated[iter->first][i];
                needed[iter->first][i] = tmp_needed[iter->first][i];
            }
        }
        return true;
    } else {
        return false;
    }

}

bool request_judge(int pid) {
    for (int i = 0; i < r; i++) {
        if (_max[pid][i] < req[i] + allocated[pid][i] || req[i] > remain[i]) {
            return false;
        }
    }
    map<int, vector<int>> tmp_needed;
    map<int, vector<int>> tmp_allocated;
    for (auto iter = needed.begin(); iter != needed.end(); iter++) {
        for (int i = 0; i < r; i++) {
            if (iter->first == pid) {
                tmp_needed[iter->first].push_back(needed[iter->first][i] - req[i]);
                tmp_allocated[iter->first].push_back(allocated[iter->first][i] + req[i]);
            } else {
                tmp_needed[iter->first].push_back(needed[iter->first][i]);
                tmp_allocated[iter->first].push_back(allocated[iter->first][i]);
            }
        }
    }
    bool res = safe_check(tmp_allocated, tmp_needed);
    return res;
}

bool new_judge(int pid) {
    if (_max.find(pid) != _max.end()) {
        return false;
    }
    for (int i = 0; i < r; i++) {
        if (req[i] > resource[i]) {
            return false;
        }
    }
    return true;
}

void new_process(int pid) {
    for (int i = 0; i < r; i++) {
        _max[pid].push_back(req[i]);
        needed[pid].push_back(req[i]);
        allocated[pid].push_back(0);
    }
}

int main() {
    // freopen("1.in", "r", stdin);
    cin >> r;
    int tmp;
    for (int i = 0; i < r; i++) {
        cin >> tmp;
        remain.push_back(tmp);
        resource.push_back(tmp);
        // leave room for req
        req.push_back(0);
    }
    bool res;
    while (cin >> pid >> str) {
        if (str != "terminate") {
            for (int i = 0; i < r; i++) {
                cin >> req[i];
            }
            if (str == "request") {
                res = request_judge(pid);
                if (res) {
                    cout << "OK" << endl;
                } else {
                    cout << "NOT OK" << endl;
                }
            } else if (str == "new") {
                res = new_judge(pid);
                if (res) {
                    cout << "OK" << endl;
                    new_process(pid);
                } else {
                    cout << "NOT OK" << endl;
                }
            }
        } else {
            if (_max.find(pid) != _max.end()) {
                recycle(pid);
                cout << "OK" << endl;
            } else {
                cout << "NOT OK" << endl;
            }
        }
#ifdef DEBUG
        printf("--------------------------\n");
        printf("-----max-----\n");
        for (auto &iter : _max) {
            for (int i = 0; i < r; i++) {
                printf("%d ", _max[iter.first][i]);
            }
            printf("\n");
        }
        printf("-----allocated-----\n");
        for (auto &iter : allocated) {
            for (int i = 0; i < r; i++) {
                printf("%d ", allocated[iter.first][i]);
            }
            printf("\n");
        }
        printf("-----needed-----\n");
        for (auto &iter : needed) {
            for (int i = 0; i < r; i++) {
                printf("%d ", needed[iter.first][i]);
            }
            printf("\n");
        }
        printf("--------------------------\n");
#endif
    }
    return 0;
}
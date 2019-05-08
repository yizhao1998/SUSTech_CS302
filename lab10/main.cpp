#include <iostream>
#include <string>
#include <cstdio>
#include <list>
#include <algorithm>

#define MAX 0x3f3f3f3f
using namespace std;
int S;
int M;
int N;

void fcfs(int *order) {
    int sum = 0;
    printf("%d -> ", S);
    sum += abs(order[0] - S);
    for (int i = 1; i < N; i++) {
        sum += abs(order[i] - order[i - 1]);
    }
    for (int i = 0; i < N - 1; i++) {
        printf("%d -> ", order[i]);
    }
    printf("%d\n", order[N - 1]);
    printf("%d\n", sum);
}

void sstf(int *order) {
    int sum = 0;
    list<int> ls;
    for (int i = 0; i < N; i++) {
        ls.push_back(order[i]);
    }
    int cur = S;
    int min = MAX;
    int pre = -1;
    int pos = -1;
    printf("%d -> ", S);
    for (int i = 0; i < N; i++) {
        for (auto iter = ls.begin(); iter != ls.end(); iter++) {
            if (abs(cur - *iter) < min) {
                min = abs(cur - *iter);
                pos = *iter;
            }
        }
        if (i == 0) {
            sum += abs(pos - S);
        } else {
            sum += abs(pos - pre);
        }
        ls.erase(find(ls.begin(), ls.end(), pos));
        if (i < N - 1) {
            printf("%d -> ", pos);
        } else {
            printf("%d\n", pos);
        }
        cur = pos;
        min = MAX;
        pre = pos;
        pos = -1;
    }
    printf("%d\n", sum);
}

void scan(int *order, int advanced) {
    int sum = 0;
    int tmp_order[N];
    for (int i = 0; i < N; i++) {
        tmp_order[i] = order[i];
    }
    sort(tmp_order, tmp_order + N);
    int pos = 0;
    for (int i = 0; i < N; i++) {
        if (tmp_order[i] > S) {
            pos = i - 1;
            break;
        }
    }
    printf("%d -> ", S);
    for (int i = pos; i >= 0; i--) {
        if (i == pos) {
            sum += abs(tmp_order[i] - S);
        } else {
            sum += abs(tmp_order[i] - tmp_order[i + 1]);
        }
        printf("%d -> ", tmp_order[i]);
    }
    if (!advanced) {
        sum += abs(0 - tmp_order[0]);
        printf("%d -> ", 0);
    }
    for (int i = pos + 1; i < N - 1; i++) {
        if (i == pos + 1) {
            if (!advanced) {
                sum += abs(tmp_order[i] - 0);
            } else {
                sum += abs(tmp_order[i] - tmp_order[0]);
            }
        } else {
            sum += abs(tmp_order[i] - tmp_order[i - 1]);
        }
        printf("%d -> ", tmp_order[i]);
    }
    sum += abs(tmp_order[N-1] - tmp_order[N - 2]);
    printf("%d\n", tmp_order[N - 1]);
    printf("%d\n", sum);
}

void c_scan(int *order, int advanced) {
    int sum = 0;
    int tmp_order[N];
    for (int i = 0; i < N; i++) {
        tmp_order[i] = order[i];
    }
    sort(tmp_order, tmp_order + N);
    int pos = 0;
    for (int i = 0; i < N; i++) {
        if (tmp_order[i] > S) {
            pos = i - 1;
            break;
        }
    }
    printf("%d -> ", S);
    for (int i = pos; i >= 0; i--) {
        if (i == pos) {
            sum += abs(tmp_order[i] - S);
        } else {
            sum += abs(tmp_order[i] - tmp_order[i + 1]);
        }
        printf("%d -> ", tmp_order[i]);
    }
    if (!advanced) {
        sum += abs(0 - tmp_order[0]);
        printf("%d -> ", 0);
        printf("%d -> ", M - 1);
    }
    for (int i = N - 1; i > pos + 1; i--) {
        if (i == N-1) {
            if (!advanced) {
                sum += abs(tmp_order[i] - (M-1));
            }
        } else {
            sum += abs(tmp_order[i] - tmp_order[i + 1]);
        }
        printf("%d -> ", tmp_order[i]);
    }
    sum += abs(tmp_order[pos+1] - tmp_order[pos+2]);
    printf("%d\n", tmp_order[pos + 1]);
    printf("%d\n", sum);
}

int main() {
//    freopen("3.in", "r", stdin);
    scanf("%d", &S);
    scanf("%d", &M);
    scanf("%d", &N);
    int *order = new int[N];
    for (int i = 0; i < N; i++) {
        scanf("%d", &order[i]);
    }
    printf("FCFS\n");
    fcfs(order);
    printf("SSTF\n");
    sstf(order);
    printf("SCAN\n");
    scan(order, 0);
    printf("C-SCAN\n");
    c_scan(order, 0);
    printf("LOOK\n");
    scan(order, 1);
    printf("C-LOOK\n");
    c_scan(order, 1);
    return 0;
}
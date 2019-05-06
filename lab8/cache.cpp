#include <cstdio>
#include <cstring>
#include <queue>
#include <map>
#include <set>
#include <iostream>
#include <cmath>
#include <list>
#include <vector>
#include <algorithm>

using namespace std;
typedef pair<int, int> pii;
int K;
int A;
int N;

int fifo(){
    int hit;
    int order[N];
    queue<int> que;
    set<int> ele_set;
    for(int i=0;i<N;i++){
        scanf("%d", &order[i]);
    }
    for(int i=0;i<N;i++){
        // miss
        if(ele_set.find(order[i]) == ele_set.end()){
            if(que.size() == K){
                int f = que.front();
                que.pop();
                set<int>::iterator iter = ele_set.find(f);
                ele_set.erase(iter);
            }
            que.push(order[i]);
            ele_set.insert(order[i]);
        }
        // hit
        else{
            hit++;
        }
        que.push(order[i]);
    }
    return hit;
}

int min(){
    int hit;
    int order[N];
    list<int> ls;
    map<int, list<int>> mls;
    map<int, list<int>::iterator> pos_map;
    for(int i=0;i<N;i++){
        scanf("%d", &order[i]);
        mls[order[i]].push_back(i);
    }
    for(int i=0;i<N;i++){
        // miss
        if(pos_map.find(order[i]) == pos_map.end()){
            if(pos_map.size() == K){
                int max_pos = -1;
                int max_num = -1;
                for(auto iter=pos_map.begin(); iter != pos_map.end(); iter++){
                    auto it=mls[iter->first].begin();
                    while(it != mls[iter->first].end()){
                        if(*it < i){
                            mls[iter->first].pop_front();
                        }
                        else{
                            if(*it > max_num){
                                max_pos = iter->first;
                                max_num = *it;
                            }
                            break;
                        }
                    }      
                }
                ls.erase(pos_map[max_pos]);
                pos_map.erase(pos_map.find(max_pos));
            }
            ls.push_back(order[i]);
            pos_map[order[i]] = --ls.end();
        }
        else{
            hit++;
        }
    }
    return hit;
}

int lru(){
    int hit;
    int order[N];
    list<int> ls;
    map<int, list<int>::iterator> pos_map;
    for(int i=0;i<N;i++){
        scanf("%d", &order[i]);
    }
    for(int i=0;i<N;i++){
        // miss
        if(pos_map.find(order[i]) == pos_map.end()){
            if(pos_map.size() == K){
                int ind = ls.front();
                ls.pop_front();
                pos_map.erase(pos_map.find(ind));
            }
            ls.push_back(order[i]);
            pos_map[order[i]] = --ls.end();
        }
        // hit
        // adjust the used list
        else{
            hit++;
            auto pos = pos_map[order[i]];
            ls.erase(pos);
            ls.push_back(order[i]);
            pos_map[order[i]] = --ls.end();
        }
    }
    return hit;
}

int get_ind(int ind){
    while(ind > K){
        ind -= K;
    }
    return ind;
}

vector<pii>::iterator _find(vector<pii> &vec, int find_ele){
    for(vector<pii>::iterator it = vec.begin(); it != vec.end(); it++){
        if(it->first == find_ele){
            return it;
        }
    }
    return vec.end();
}

int _clock(){
    int hit;
    int order[N];
    int hand = 1;
    vector<pii> vec(N+1, {-1, 0});
    for(int i=0;i<N;i++){
        scanf("%d", &order[i]);
    }
    for(int i=0;i<N;i++){
        vector<pii>::iterator iter = _find(vec, order[i]);
        // miss
        if(iter == vec.end()){
            while(true){
                if(vec[hand].second == 0){
                    vec[hand].first = order[i];
                    vec[hand].second = 1;
                    break;
                }
                vec[hand].second = 0;
                hand = get_ind(++hand);
            }
        }
        // hit
        else{
            iter->second = 1;
        }
    }
}

int second_chance(){

}

int main(){
    // cache size
    scanf("%d", &K);
    scanf("%d", &A);
    scanf("%d", &N);
    int hit;
    switch(A){
        // fifo
        case 1:
            hit = fifo();
            break;
        // min
        case 2:
            min();
            break;
        // lru
        case 3:
            lru();
            break;
        // clock
        case 4:
            _clock();
            break;
        // second_chance
        case 5:
            second_chance();
            break;
    }
}
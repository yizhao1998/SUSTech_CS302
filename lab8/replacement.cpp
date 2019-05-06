#include <algorithm>
#include <list>
#include <map>
#include <stdio.h>
#include <vector>

using namespace std;

struct clock_stru {
    int valid = 0;
    int usage = 0;
    int value;
};

int main(int argc, char const* argv[])
{
    int cache_size;
    int algorithm;
    int num;
    int index;
    int hit = 0;
    scanf("%d\n", &cache_size);
    scanf("%d\n", &algorithm);
    scanf("%d\n", &num);

    if (algorithm == 0) {
        list<int> fifo_cache;
        list<int>::iterator iter;
        for (int i = 0; i < num; i++) {
            scanf("%d ", &index);
            iter = find(fifo_cache.begin(), fifo_cache.end(), index);
            if (iter != fifo_cache.end()) { // element exist
                hit++;
            } else {
                fifo_cache.push_back(index);
                if (fifo_cache.size() > cache_size) {
                    fifo_cache.pop_front();
                }
            }
        }
    } else if (algorithm == 1) {
        map<int, int> lru_cache;
        map<int, int>::iterator iter;
        for (int i = 0; i < num; i++) {
            scanf("%d ", &index);
            iter = lru_cache.find(index);
            lru_cache[index] = i;
            if (iter != lru_cache.end()) {
                hit++;
            } else {
                if (lru_cache.size() > cache_size) {
                    map<int, int>::iterator min_iter = lru_cache.begin();
                    for (iter = lru_cache.begin(); iter != lru_cache.end(); iter++) {
                        if (iter->second < min_iter->second) {
                            min_iter = iter;
                        }
                    }
                    lru_cache.erase(min_iter);
                }
            }
        }

    } else if (algorithm == 2) {
        map<int, list<int>> info;
        map<int, int> min_cache;
        map<int, list<int>>::iterator iter;
        map<int, int>::iterator min_iter;
        vector<int> arr;
        arr.assign(num, 0);
        for (int i = 0; i < num; i++) {
            scanf("%d ", &index);
            arr[i] = index;

            iter = info.find(arr[i]);
            if (iter != info.end()) {
                iter->second.push_back(i);
            } else {
                info[index].push_back(i);
            }
        }
        // treate the last element as infinity
        for (iter = info.begin(); iter != info.end(); iter++) {
            iter->second.push_back(num + 10);
        }

        for (int i = 0; i < num; i++) {
            min_iter = min_cache.find(arr[i]);
            if (min_iter != min_cache.end()) {
                hit++;
            } else {
                min_cache[arr[i]] = 0;
                if (min_cache.size() > cache_size) {
                    map<int, int>::iterator max = min_cache.begin();
                    for (min_iter = min_cache.begin(); min_iter != min_cache.end(); min_iter++) {
                        int page = min_iter->first;
                        while (info[page].front() < i) {
                            info[page].pop_front();
                        }
                        if (info[max->first].front() < info[page].front()) {
                            max = min_iter;
                        }
                    }
                    min_cache.erase(max);
                }
            }
        }
    } else if (algorithm == 3) {
        list<clock_stru> clock_cache;
        list<clock_stru>::iterator iter;
        list<clock_stru>::iterator hand;
        for (int i = 0; i < num; i++) {
            scanf("%d\n", &index);
            for (iter = clock_cache.begin(); iter != clock_cache.end(); iter++) {
                if (iter->valid == 1 && iter->value == index) {
                    break;
                }
            }
            if (iter != clock_cache.end()) {
                iter->usage = 1;
                hit++;
            } else {
                if (clock_cache.size() < cache_size - 1) {
                    clock_stru stru;
                    stru.valid = 1;
                    stru.usage = 1;
                    stru.value = index;
                    clock_cache.push_back(stru);
                    hand = clock_cache.begin();
                } else {
                    bool victum = true;
                    while (victum) {
                        if (hand->valid == 1 && hand->usage == 0) { //replacement
                            hand->value = index;
                            hand->usage = 1;
                            victum = false;
                        } else {
                            hand->usage = 0;
                        }
                        hand++;
                        if (hand == clock_cache.end()) {
                            hand = clock_cache.begin();
                        }
                    }
                }
            }
        }

    } else if (algorithm == 4) {
        for (int i = 0; i < num; i++) {
            scanf("%d\n", &index);
        }
    }
    // printf("%d\n", hit);
    printf("Hit ratio = %.2f%\n", hit * 1.0 / num * 100);
    return 0;
}

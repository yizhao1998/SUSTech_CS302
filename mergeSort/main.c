#include "myMerge.h"

int main() {
    thread_num = 0;
    freopen("1.out", "w", stdout);
    srand(time(NULL));
    for (int i = 0; i < MAX_ARRAY_SIZE; ++i) {
        arr[i] = rand() % 40 - 20;
    }
    int len = MAX_ARRAY_SIZE / MAX_THREAD_NUM;
    int l = 0;
    pthread_t threads[MAX_THREAD_NUM];
    struct node nodelist[MAX_THREAD_NUM];
    for (int i = 0; i < MAX_THREAD_NUM; i++, l += len) {
        nodelist[i].l = l;
        nodelist[i].r = l + len - 1;
    }
    nodelist[MAX_THREAD_NUM-1].r = MAX_ARRAY_SIZE - 1;
    
    struct timeval start;
    struct timeval end;
    float timer;
    long start_time = get_current_time();
    for (int i = 0; i < MAX_THREAD_NUM; i++) {
        pthread_create(&threads[i], NULL, merge_sort, &nodelist[i]);
    }

    for (int i = 0; i < MAX_THREAD_NUM; i++) {
        pthread_join(threads[i], NULL);
    } 
    for (int i = 1; i < MAX_THREAD_NUM; i++) {
        merge(0, nodelist[i].l-1, nodelist[i].r);
    }
    for (int i = 0; i < MAX_ARRAY_SIZE; ++i) {
        printf("%d ", arr[i]);
    }
    // printf("\n");
    long totaltime = get_current_time() - start_time;
    printf("此程序的运行时间为%lld ms！", totaltime);
    pthread_exit(NULL);
    return 0;
}

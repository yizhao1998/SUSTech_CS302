#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#define MAX_ARRAY_SIZE 30
int arr[MAX_ARRAY_SIZE];

struct node {
    int l;
    int r;
};

void merge(const int l, const int r) {
    int mid = (l + r) / 2;
    int iter_1 = l;
    int iter_2 = mid + 1;
    int new_arr[r - l + 1];
    int iter = 0;
    while (iter_1 <= mid && iter_2 <= r) {
        if (arr[iter_1] < arr[iter_2]) {
            new_arr[iter++] = arr[iter_1++];
        } else {
            new_arr[iter++] = arr[iter_2++];
        }
    }
    if (iter_1 <= mid) {
        while (iter_1 <= mid) {
            new_arr[iter++] = arr[iter_1++];
        }
    } else {
        while (iter_2 <= r) {
            new_arr[iter++] = arr[iter_2++];
        }
    }
    for (int i = l; i <= r; ++i) {
        arr[i] = new_arr[i - l];
    }
}

void *merge_sort(void *args) {
    struct node *n = (struct node *) args;
    int l = n->l;
    int r = n->r;
    if (l >= r) {
        return NULL;
    }
    int mid = (l + r) / 2 + 1;
    pthread_t tid_1, tid_2;
    struct node *n1 = (struct node *) malloc(sizeof(struct node *));
    struct node *n2 = (struct node *) malloc(sizeof(struct node *));
    n1->l = l;
    n1->r = mid - 1;
    n2->l = mid;
    n2->r = r;
    int err;
    err = pthread_create(&tid_1, NULL, merge_sort, n1);
    if (err)
        printf("can't create thread: %s\n", strerror(err));
    err = pthread_create(&tid_2, NULL, merge_sort, n2);
    if (err)
        printf("can't create thread: %s\n", strerror(err));
    pthread_join(tid_1, NULL);
    pthread_join(tid_2, NULL);
    merge(l, r);
    free(n1);
    free(n2);
}
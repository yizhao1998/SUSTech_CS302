#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#define MAX_ARRAY_SIZE 10000
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
    struct node *n1 = (struct node *) malloc(sizeof(struct node *));
    struct node *n2 = (struct node *) malloc(sizeof(struct node *));
    n1->l = l;
    n1->r = mid - 1;
    n2->l = mid;
    n2->r = r;
    merge_sort(n1);
    merge_sort(n2);
    merge(l, r);
    free(n1);
    free(n2);
    return 0;
}

int main() {
    freopen("1.out", "w", stdout);
    clock_t start,finish;
    double totaltime;
    start=clock();
    struct node *n = (struct node *) malloc(sizeof(struct node *));
    srand(time(NULL));
    printf("Initial Array:\n");
    for (int i = 0; i < MAX_ARRAY_SIZE; ++i) {
        arr[i] = rand() % 40 - 20;
        printf("%d ", arr[i]);
    }
    printf("\n");
    n->l = 0;
    n->r = MAX_ARRAY_SIZE - 1;
    merge_sort(n);
    printf("Sorted Array:\n");
    for (int i = 0; i < MAX_ARRAY_SIZE; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    free(n);
    finish=clock();
    totaltime=(double)(finish-start);
    printf("\n此程序的运行时间为%d us！", totaltime);
    printf("\nsinglemerge");
    return 0;
}
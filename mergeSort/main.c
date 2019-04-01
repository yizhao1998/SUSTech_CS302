#include "myMerge.h"

int main() {
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
    pthread_t tid;
    int err;
    err = pthread_create(&tid, NULL, merge_sort, n);
    if (err) {
        printf("can't create thread: %s\n", strerror(err));
    }
    pthread_join(tid, NULL);
    printf("Sorted Array:\n");
    for (int i = 0; i < MAX_ARRAY_SIZE; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    free(n);
    return 0;
}

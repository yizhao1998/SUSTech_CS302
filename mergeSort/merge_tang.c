#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>
#define maxn 100000010
// #define SINGLE_THREAD
#define MULTI_THREAD
#ifndef DEBUG
#define DEBUG
#endif

int max_thread_num;
int A[maxn];
int temp[maxn];
int B[maxn];
int n;

int thread_num = 0;

struct atom {
	int l, r;
};

int cmp(const void *lhs, const void *rhs);

long get_current_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void merge(int l, int r) {
	int mid = (l + r) >> 1;
    memcpy(temp + l, A + l, sizeof(int) * (r - l));
    int i = l, j = mid, k = l;
	while(i < mid && j < r) {
		if(temp[i] < temp[j]) A[k++] = temp[i++];
		else A[k++] = temp[j++];
	}
	memcpy(temp + i, A + k, sizeof(int) * (mid - i));
	k += (mid - i);
	memcpy(temp + j, A + k, sizeof(int) * (r - j));
}

void* my_merge_sort(void *node) {
	struct atom* p = (struct atom*)node;
	int l = p->l, r = p->r;
    if(r - l <= 5) {
        qsort(A + l, r - l, sizeof(int), cmp);
        return NULL;
    }
    if(r - l <= 1) return NULL;

	int mid = (l + r) >> 1;
	
	struct atom left, right;
	left.l = l;
	left.r = mid;
	right.l = mid;
	right.r = r;
	
#ifdef SINGLE_THREAD
	my_merge_sort((void *)&left);
	my_merge_sort((void *)&right);
#endif
	
#ifdef MULTI_THREAD
	if ((((thread_num + 1) >> 1) >= max_thread_num)) {
		my_merge_sort((void *)&left);
        my_merge_sort((void *)&right);
    } else {
        pthread_t tid1, tid2;
		thread_num += 2;
        if (pthread_create(&tid1, NULL, my_merge_sort, ((void*)(&left))) != 0) {
            fprintf(stderr, "thread create error\n");
            perror("");
		}    
		if(pthread_create(&tid2, NULL, my_merge_sort, ((void*)(&right))) != 0) {
            fprintf(stderr, "thread create error\n");
			perror("");
		}
		pthread_join(tid1, NULL);
	    pthread_join(tid2, NULL);
    }
#endif
	merge(l, r);
    return NULL;
}

int cmp(const void *lhs, const void *rhs) {
	return *(int *)lhs - *(int *)rhs;
}

int check(int arr[], int size) {
    qsort(B, size, sizeof(int), cmp);
	for(int i = 0; i < size; i++) {
		if(arr[i] != B[i]) return 0;
	}
	return 1;
}

void test(int A[], int size) {
	srand(time(NULL));
	n = size;
	for(int i = 0; i < size; i++) {
		A[i] = rand();
	}
	memcpy(B, A, sizeof(int) * n);

	struct atom p;
	p.l = 0;
	p.r = n;
    long start_timer = get_current_time();
#ifdef MULTI_THREAD	
	pthread_t tid;
	if(pthread_create(&tid, NULL, my_merge_sort, (void*)&p) != 0) perror("");
	else thread_num++;
	pthread_join(tid, NULL);
#endif
#ifdef SINGLE_THREAD
	my_merge_sort((void *)&p);
#endif
    long end_timer = get_current_time();
	
	for(int i = 0; i < size; i++) {
		printf("%d ", A[i]);
	}
	printf("\n");
	
	printf("size of array: %d\n", n);
	printf("number of thread: %d\n", thread_num / 2 + 1);
	printf("result %s\n", check(A, n) ? "true" : "false");
    printf("time: %ldms\n", end_timer - start_timer);
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	if(argc != 3) {
		printf("Command format $ ./mergesort <size_of_array> <thread_num>\n");
		return 0;
	}
	int size = atoi(argv[1]);
	max_thread_num = atoi(argv[2]);
	test(A, 100000000);
	return 0;
}
#include <pthread.h>

#include <stdio.h>

#include <stdlib.h>

#include <time.h>

#include <string.h>



int MAX = 10000000;

int THREAD_MAX = 8;

int *a;

int A[10000000];

int B[10000000];



struct tsk {

  int l;

  int h;

};



void merge(int low, int mid, int high) {

  memcpy(B + low, A + low, sizeof(int) * (high - low + 1));

  int i = low;

  int j = mid + 1;

  int k = low;

  while (i <= mid && j <= high) {

    if (B[i] <= B[j])

      A[k++] = B[i++];

    else

      A[k++] = B[j++];

  }

  while (i <= mid) A[k++] = B[i++];

  while (j <= high) A[k++] = B[j++];

}



void merge_sort(int low, int high) {

  int mid = (high + low) / 2;

  if (low < high) {

    merge_sort(low, mid);

    merge_sort(mid + 1, high);

    merge(low, mid, high);

  }

}



void *multi_merge_sort(void *arg) {

  struct tsk *tsk = arg;

  int low;

  int high; 

  low = tsk->l;

  high = tsk->h;

  int mid = low + (high - low) / 2;

  if (low < high) {

    merge_sort(low, mid);

    merge_sort(mid + 1, high);

    merge(low, mid, high);

  }

  return 0;

}



int main() {

  struct tsk *temp;

  for (int i = 0; i < MAX; i++) {

      A[i] = rand() % 40 - 20;

  }

  pthread_t threads[THREAD_MAX];

  struct tsk tsklist[THREAD_MAX];

  int len = MAX / THREAD_MAX;

  int low = 0;

  for (int i = 0; i < THREAD_MAX; i++, low += len) {

    temp = &tsklist[i];

    temp->l = low;

    temp->h = low + len - 1;

  }

  temp->h = MAX - 1;

  

  struct timeval start;

  struct timeval end;

  float timer;

  gettimeofday(&start, NULL);



  for (int i = 0; i < THREAD_MAX; i++) {

    temp = &tsklist[i];

    pthread_create(&threads[i], NULL, multi_merge_sort, temp);

  }



  for (int i = 0; i < THREAD_MAX; i++) 

    pthread_join(threads[i], NULL);

  struct tsk *tskm = &tsklist[0];

  for (int i = 1; i < THREAD_MAX; i++) {

    temp = &tsklist[i];

    merge(tskm->l, temp->l - 1, temp->h);

  }



  printf("\nSorted array:");

  for (int i = 0; i < MAX; i++) 

    printf(" %d", A[i]);



    gettimeofday(&end, NULL);

    timer = end.tv_sec - start.tv_sec +

            (float)(end.tv_usec - start.tv_usec) / 1000000;

    printf("\ntimer = %fs\n", timer);



    // int arr[] = {1, 2, 3, 4, 5};

    // int copy[5];

    // int len3 = sizeof(arr) / sizeof(arr[0]);

    // memcpy(copy + 1, arr + 1, len3 * sizeof(int));  // 输出 1,2,3,4,5

    // for(int i = 0; i < 5; i++)

    //   printf("%d ", copy[i]);

    return 0;

}

#define _XOPEN_SOURCE 500
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "cthreadpool.h"

#define NUM_THREADS 4
#define DEFAULT_QUEUE_LENGTH 15
#define NUM_TASKS 20

void slow_square(int n) {
  unsigned int sleepy_time = (unsigned int)rand() % 2000000;
  usleep(sleepy_time);
  printf("After sleeping for %f, the value of %d squared is %d\n", 
         (double)sleepy_time/1000000, n, n*n);
}

void function_wrapper(void *arg) {
  int *num = (int *)arg;
  slow_square(*num);
}

int main(void) {
  srand((unsigned int)time(0));

  sdm_threadpool_t *pool = sdm_threadpool_create(NUM_THREADS, DEFAULT_QUEUE_LENGTH);

  int task_num[NUM_TASKS] = {0};
  for (int i = 0; i < NUM_TASKS; i++) {
    task_num[i] = i;
    sdm_threadpool_add(pool, function_wrapper, &(task_num[i]));
  }

  sdm_threadpool_join(pool);
  return 0;
}


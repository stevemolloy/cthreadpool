#define _XOPEN_SOURCE 500
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "cthreadpool.h"

#define NUM_THREADS 4
#define DEFAULT_QUEUE_LENGTH 8
#define NUM_TASKS 20

int slow_square(int n) {
  unsigned int sleepy_time = (unsigned int)rand() % 1000000;
  printf("n = %2d :: Sleeping for %0.3f s\n", n, (double)sleepy_time/1000000);
  usleep(sleepy_time);
  int n_sqr = n*n;
  return n_sqr;
}

typedef struct {
  int input;
  int output;
} WrapperArg;

void function_wrapper(void *arg) {
  WrapperArg *cast_arg = (WrapperArg *)arg;

  cast_arg->output = slow_square(cast_arg->input);
}

int main(void) {
  srand((unsigned int)time(0));

  sdm_threadpool_t *pool = sdm_threadpool_create(NUM_THREADS, DEFAULT_QUEUE_LENGTH);

  WrapperArg arg[NUM_TASKS] = {0};
  printf("Submitting job ");
  for (int i = 0; i < NUM_TASKS; i++) {
    arg[i].input = i;
    printf("%d", i);
    if (i != NUM_TASKS-1) printf(", ");
    sdm_threadpool_add(pool, function_wrapper, &(arg[i]));
  }
  printf("\n");

  sdm_threadpool_join(pool);

  for (size_t i=0; i<NUM_TASKS; i++) {
    printf("arg[%zu].input = %d :: arg[%zu].output = %d\n",
           i, arg[i].input, i, arg[i].output);
  }

  return 0;
}


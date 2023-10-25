#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cthreadpool.h"

#define NUMJOBS 10
#define NUMTHREADS 10
#define SLEEP_US 1000 * 1000

typedef struct {
  int in_val;
  int out_val;
} WrapperInput;

int slow_square(int val) {
  usleep(SLEEP_US);
  return val * val;
}

void *wrapper(void *v_ptr) {
  WrapperInput *wi = (WrapperInput *)v_ptr;

  wi->out_val = slow_square(wi->in_val);

  return NULL;
}

int main(void) {
  Pool threadpool = create_new_pool(NUMTHREADS);

  WrapperInput wi_s[NUMJOBS];
  Job jobs[NUMJOBS];
  for (size_t i=0; i<NUMJOBS; i++) {
    wi_s[i].in_val = (int)i+0;
    jobs[i].fun_ptr = &wrapper;
    jobs[i].data_struct = (void*)&wi_s[i];
  }

  for (size_t i=0; i<NUMJOBS; i++) {
    int result = add_job_to_queue(&(threadpool.queue), jobs[i]);
    if (result < 0) {
      fprintf(stderr, "Could not add job to queue. Exiting\n");
      return 1;
    }
  }

  printf("Running all jobs\n");
  run_pool_to_completion(&threadpool);

  printf("Tearing down the pool\n");
  kill_pool(&threadpool);

  printf("\n=====================\n\n");
  for (size_t i=0; i<NUMJOBS; i++) {
    printf("Result from thread #%zu = %d\n", i, wi_s[i].out_val);
  }

  return 0;
}


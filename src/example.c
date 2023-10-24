#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMJOBS 15
#define NUMTHREADS 5
#define SLEEP_US 500 * 1000

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
  // For this to work we need an array of threads and an array of structs used as inputs
  pthread_t threads[NUMJOBS];
  WrapperInput inputs[NUMJOBS];

  // Prepare the input and create the threads
  for (int i = 0; i < NUMJOBS; i++) {
    inputs[i].in_val = i;
    pthread_create(&threads[i], NULL, &wrapper, &inputs[i]);
  }

  // Wait for the threads to complete
  for (int i = 0; i < NUMJOBS; i++) {
      pthread_join(threads[i], NULL);
  }

  // Print results
  for (int i = 0; i < NUMJOBS; i++) {
      printf("%d squared is %d\n", i * 1, inputs[i].out_val);
  }

  return 0;
}


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMJOBS 10

int slow_square(int val) {
  usleep(500000);

  return val*val;
}

void *wrapper(void *v_ptr) {
  int val = *(int *)v_ptr;

  int *res = malloc(sizeof(int));

  *res = slow_square(val);

  return (void*)res;
}

int main(void) {
  // pthread_t threads[NUMJOBS];

  for (int i=0; i<NUMJOBS; i++) {
    int input = i * 1;
    void *v_ptr = (void*)&input;

    void *res_ptr = wrapper(v_ptr);

    int *res = (int*)res_ptr;

    printf("%d squared is %d\n", input, *res);
  }

  return 0;
}

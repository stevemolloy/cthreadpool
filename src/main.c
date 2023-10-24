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
    int val = wi->in_val;

    wi->out_val = slow_square(val);

    return NULL;
}

int main(void) {
    pthread_t threads[NUMJOBS];
    WrapperInput inputs[NUMJOBS];

    for (int i = 0; i < NUMJOBS; i++) {
      inputs[i].in_val = i;
      pthread_create(&threads[i], NULL, &wrapper, &inputs[i]);
    }

    for (int i = 0; i < NUMJOBS; i++) {
        pthread_join(threads[i], NULL);

        printf("%d squared is %d\n", i * 1, inputs[i].out_val);
    }

    return 0;
}


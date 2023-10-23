#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMJOBS 100
#define NUMTHREADS 5
#define SLEEP_US 500000

int slow_square(int val) {
    usleep(SLEEP_US);
    return val * val;
}

void *wrapper(void *v_ptr) {
    int *val_ptr = (int *)v_ptr;
    int val = *val_ptr;

    int *res = malloc(sizeof(int));
    *res = slow_square(val);
    return (void *)res;
}

int main(void) {
    pthread_t threads[NUMJOBS];

    int inputs[NUMJOBS];
    for (int i = 0; i < NUMJOBS; i++) {
        inputs[i] = i * 1;
        pthread_create(&threads[i], NULL, &wrapper, &inputs[i]);
    }

    void *res_ptr[NUMJOBS];

    for (int i = 0; i < NUMJOBS; i++) {
        pthread_join(threads[i], &res_ptr[i]);

        int *res = (int *)res_ptr[i];

        printf("%d squared is %d\n", i * 1, *res);
        free(res); // Free the allocated memory
    }

    return 0;
}


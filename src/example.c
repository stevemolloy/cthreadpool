#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMJOBS 15
#define NUMTHREADS 5
#define SLEEP_US 1500 * 1000

typedef struct {
  void *(*fun_ptr)(void*); // A pointer to a function that takes a void* as input and returns a void*
  void *data_struct; // The void* to be used as input to the function
  pthread_t thread;
} Job;

typedef struct {
  size_t capacity;
  size_t length;
  Job *jobs;
} JobQueue;

typedef struct {
  size_t capacity;
  size_t length;
  Job *jobs;
} ActiveWorkers;

typedef struct {
  size_t max_workers;
  JobQueue queue;
  ActiveWorkers active_workers;
} Pool;

void wait_for_queue(JobQueue *queue) {
  for (size_t i=0; i<queue->length; i++) {
    pthread_join(queue->jobs[i].thread, NULL);
  }
}

JobQueue create_new_queue(size_t max_workers) {
  Job *joblist = malloc(max_workers * sizeof(Job));
  return (JobQueue) {
    .capacity = max_workers,
    .length = 0,
    .jobs = joblist,
  };
}

int add_job_to_queue(JobQueue *queue, Job newjob) {
  if (queue->length < queue->capacity) {
    queue->jobs[queue->length++] = newjob;
    return 0;
  } else {
    queue->jobs = realloc(queue->jobs, queue->capacity * 2);
    if (queue->jobs == NULL) {
      fprintf(stderr, "Could not add the new job");
      return -1;
    }
    queue->capacity *= 2;
    return 0;
  }
}

ActiveWorkers create_new_workers_list(size_t max_workers) {
  Job *joblist = malloc(max_workers * sizeof(Job));
  ActiveWorkers aws = (ActiveWorkers) {
    .capacity = max_workers,
    .length = 0,
    .jobs = joblist,
  };
  return aws;
}

int add_job_to_activeworkers(ActiveWorkers *aws, Job newjob) {
  if (aws->length < aws->capacity) {
    aws->jobs[aws->length] = newjob;
    pthread_create(
        &aws->jobs[aws->length].thread,
        NULL,
        aws->jobs[aws->length].fun_ptr,
        aws->jobs[aws->length].data_struct
      );
    aws->length++;
    return 1;
  } else {
    return -1;
  }
}

Pool create_new_pool(size_t max_workers) {
  return (Pool) {
    .max_workers = max_workers,
    .queue = create_new_queue(64),
    .active_workers = create_new_workers_list(max_workers),
  };
}

int run_pool_to_completion(Pool *pool) {
  size_t max_workers = pool->active_workers.capacity;

  while (pool->queue.length > 0) {
    
    while (pool->active_workers.length < max_workers) {
      if (pool->queue.length == 0) {
        break;
      }
      Job *next_job = malloc(sizeof(Job));
      *next_job = pool->queue.jobs[--pool->queue.length];
      add_job_to_activeworkers(&pool->active_workers, *next_job);
      // pthread_create(
      //     &pool->active_workers.jobs[pool->active_workers.length-1].thread,
      //     NULL,
      //     pool->active_workers.jobs[pool->active_workers.length-1].fun_ptr,
      //     pool->active_workers.jobs[pool->active_workers.length-1].data_struct
      //   );
    }
  }

  return 0;
}

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
  size_t num_jobs_to_submit = 4;
  Pool threadpool = create_new_pool(4);

  WrapperInput wi_s[num_jobs_to_submit];
  Job jobs[num_jobs_to_submit];
  for (size_t i=0; i<num_jobs_to_submit; i++) {
    wi_s[i].in_val = (int)i+2;
    jobs[i].fun_ptr = &wrapper;
    jobs[i].data_struct = (void*)&wi_s[i];
  }

  for (size_t i=0; i<num_jobs_to_submit; i++) {
    int result = add_job_to_queue(&(threadpool.queue), jobs[i]);
    if (result < 0) {
      fprintf(stderr, "Could not add job to queue. Exiting\n");
      return 1;
    }
  }

  printf("Running all jobs\n");
  run_pool_to_completion(&threadpool);

  for (size_t i=0; i<num_jobs_to_submit; i++) {
    pthread_join(threadpool.active_workers.jobs[i].thread, NULL);
  }

  for (size_t i=0; i<num_jobs_to_submit; i++) {
    printf("Result from thread = %d\n", wi_s[i].out_val);
  }

  return 0;
}


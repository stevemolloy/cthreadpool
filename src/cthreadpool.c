#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "cthreadpool.h"

#define DEF_QUEUE_SIZE 64

JobQueue create_new_queue(size_t max_workers) {
  Job *joblist = malloc(max_workers * sizeof(Job));
  return (JobQueue) {
    .capacity = max_workers,
    .length = 0,
    .jobs = joblist,
  };
}

int add_job_to_queue(JobQueue *queue, Job newjob) {
  if (queue->length >= queue->capacity) {
    queue->jobs = realloc(queue->jobs, sizeof(Job) * queue->capacity * 2);
    if (queue->jobs == NULL) {
      fprintf(stderr, "Could not add the new job");
      return -1;
    }
    queue->capacity *= 2;
  }
  queue->jobs[queue->length++] = newjob;
  return 0;
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
    .queue = create_new_queue(DEF_QUEUE_SIZE),
    .active_workers = create_new_workers_list(max_workers),
  };
}

int run_pool_to_completion(Pool *pool) {
  size_t max_workers = pool->active_workers.capacity;

  while (pool->queue.length > 0) {
    printf(
        "Triggering %zu jobs from the queue...\n",
        pool->queue.length > max_workers ? max_workers : pool->queue.length
      );
    while (pool->active_workers.length < max_workers) {
      if (pool->queue.length == 0) {
        break;
      }
      pool->queue.length--;
      add_job_to_activeworkers(
          &pool->active_workers,
          pool->queue.jobs[pool->queue.length]
        );
    }

    printf("Waiting for these to finish...\n");
    while (pool->active_workers.length > 0) {
      size_t index = pool->active_workers.length-1;
      pthread_join(pool->active_workers.jobs[index].thread, NULL);
      printf("\tFinished thread %ld\n", pool->active_workers.jobs[index].thread);
      pool->active_workers.length--;
    }
  }

  return 0;
}

void kill_pool(Pool *pool) {
  free(pool->queue.jobs);
  free(pool->active_workers.jobs);
}


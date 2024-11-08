#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cthreadpool.h"

void *sdm_threadpool_thread(void *threadpool);

sdm_threadpool_t *sdm_threadpool_create(size_t num_threads, size_t queue_capacity) {
  sdm_threadpool_t *pool = NULL;
  pool = malloc(sizeof(sdm_threadpool_t));
  if (pool == NULL) {
    fprintf(stderr, "Memory allocation problem. Quiting\n");
    exit(1);
  }
  memset(pool, 0, sizeof(sdm_threadpool_t));

  pool->num_threads = num_threads;
  pool->queue_capacity = queue_capacity;

  pool->threads = malloc(sizeof(pthread_t) * num_threads);
  if (pool->threads == NULL) {
    fprintf(stderr, "Memory allocation problem. Quiting\n");
    exit(1);
  }
  memset(pool->threads, 0, num_threads * sizeof(pool->threads[0]));

  pool->task_queue = malloc(sizeof(sdm_threadpool_task_t) * queue_capacity);
  if (pool->task_queue == NULL) {
    fprintf(stderr, "Memory allocation problem. Quiting\n");
    exit(1);
  }
  memset(pool->task_queue, 0, sizeof(sdm_threadpool_task_t) * queue_capacity);

  pthread_mutex_init(&(pool->lock), NULL);
  pthread_cond_init(&(pool->notify), NULL);

  for (size_t i = 0; i < num_threads; i++) {
    pthread_create(&(pool->threads[i]), NULL, &sdm_threadpool_thread, (void*)pool);
  }

  return pool;
}

void sdm_threadpool_add(sdm_threadpool_t *pool, void (*function)(void *), void *arg) {
  pthread_mutex_lock(&(pool->lock));

  while (pool->queue_length >= (pool->queue_capacity - 1)) {
    pool->queue_capacity *= 2;
    size_t new_size = sizeof(sdm_threadpool_task_t) * pool->queue_capacity;
    sdm_threadpool_task_t *new_task_queue = realloc(pool->task_queue, new_size);
    if (new_task_queue == NULL) {
      fprintf(stderr, "Memory allocation problem. Quiting\n");
      exit(1);
    }
    pool->task_queue = new_task_queue;
  }

  pool->task_queue[pool->queue_length].function = function;
  pool->task_queue[pool->queue_length].arg = arg;
  pool->queue_length++;
  pool->waiting_in_queue++;

  pthread_cond_broadcast(&(pool->notify));
  pthread_mutex_unlock(&(pool->lock));
}

void *sdm_threadpool_thread(void *threadpool) {
  sdm_threadpool_t *pool = (sdm_threadpool_t *)threadpool;

  while (1) {
    pthread_mutex_lock(&(pool->lock));

    while ((pool->waiting_in_queue == 0) && (!pool->shutdown)) {
      pthread_cond_wait(&(pool->notify), &(pool->lock));
    }

    if (pool->shutdown) {
      pthread_mutex_unlock(&(pool->lock));
      pthread_exit(NULL);
    }

    void (*function)(void *) = pool->task_queue[pool->next_in_queue].function;
    void *arg = pool->task_queue[pool->next_in_queue].arg;
    pool->next_in_queue++;
    pool->waiting_in_queue--;

    pthread_mutex_unlock(&(pool->lock));

    function(arg);
  }
}

void sdm_threadpool_join(sdm_threadpool_t *pool) {
  while (pool->waiting_in_queue > 0) {
    usleep(1000);
  }

  sdm_threadpool_destroy(pool);
}

void sdm_threadpool_destroy(sdm_threadpool_t *pool) {
  pthread_mutex_lock(&(pool->lock));

  pool->shutdown = true;

  pthread_cond_broadcast(&(pool->notify));
  pthread_mutex_unlock(&(pool->lock));

  for (size_t i = 0; i < pool->num_threads; i++) {
    pthread_join(pool->threads[i], NULL);
  }

  free(pool->threads);
  free(pool->task_queue);
  pthread_mutex_destroy(&(pool->lock));
  pthread_cond_destroy(&(pool->notify));
  free(pool);
}


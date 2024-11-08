#ifndef _CTHREADPOOL_H
#define _CTHREADPOOL_H

#include <stdbool.h>
#include <pthread.h>

// Task structure (a function and its argument)
typedef struct {
  void (*function)(void *);
  void *arg;
} sdm_threadpool_task_t;

// Thread pool structure
typedef struct {
  pthread_mutex_t lock;
  pthread_cond_t notify;
  pthread_t *threads;
  sdm_threadpool_task_t *task_queue;
  size_t num_threads;
  size_t queue_capacity;
  size_t next_in_queue;
  size_t queue_length;
  size_t waiting_in_queue;
  bool shutdown;
} sdm_threadpool_t;

sdm_threadpool_t *sdm_threadpool_create(size_t num_threads, size_t queue_size);
void sdm_threadpool_add(sdm_threadpool_t *pool, void (*function)(void *), void *arg);
void sdm_threadpool_join(sdm_threadpool_t *pool);
void sdm_threadpool_destroy(sdm_threadpool_t *pool);

#endif // !_CTHREADPOOL_H


#ifndef _CTHREADPOOL_H
#define _CTHREADPOOL_H

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

JobQueue create_new_queue(size_t);
int add_job_to_queue(JobQueue*, Job);
ActiveWorkers create_new_workers_list(size_t);
int add_job_to_activeworkers(ActiveWorkers*, Job);
Pool create_new_pool(size_t);
int run_pool_to_completion(Pool*);
void kill_pool(Pool*);

#endif // !_CTHREADPOOL_H


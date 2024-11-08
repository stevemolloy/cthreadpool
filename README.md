# A Simple ThreadPool library for C

## Quick start
Copy the `cthreadpool.c` and `cthreadpool.h` files into your project.

# Example

See `example.c`.

# Usage

## Create the thread pool

`sdm_threadpool_t *sdm_threadpool_create(size_t num_threads, size_t queue_size);`

Creates a running pool of `num_threads` threads and an empty task queue with an initial capacity of `queue_size` jobs.

## Add jobs to the queue

`void sdm_threadpool_add(sdm_threadpool_t *pool, void (*function)(void *), void *arg);`

Add a job to threadpool `pool`.  This is done by specifying the `function` that is to be called and any arguments, `arg`.  As seen in `example.c`, the `*arg` pointer may also contain an entry for any values returned from the wrapped function.

## Wait for all jobs to run

`void sdm_threadpool_join(sdm_threadpool_t *pool);`

This will block until all jobs in the queue have been run and all the threads have finished.

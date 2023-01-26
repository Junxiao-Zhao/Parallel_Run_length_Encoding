#ifndef _THREAD_H_
#define _THREAD_H_

#include "global.h"

typedef struct
{
    // Mutex & Condition Variable
    pthread_mutex_t lock;           // threadpool lock
    pthread_cond_t queue_not_empty; // for worker threads
    pthread_cond_t queue_empty;     // for main thread

    // Thread Pool
    pthread_t *workers; // worker threads list
    int num_workers;    // number of maximum worker threads (-j)

    // Task Queue
    encoding_chunk **task_queue;
    int head;
    int tail;
    int size;

    // Result Queue
    encoding_chunk **result_queue;
    int result_size;
} threadpool_t;

threadpool_t *init_pool(int num_workers);
void *worker_thread(void *pool);
int free_pool(threadpool_t *pool);

#endif
#include "global.h"

// Initialize threadpool
threadpool_t *init_pool(int num_workers)
{
  threadpool_t *pool = (threadpool_t *)malloc(sizeof(threadpool_t));

  // Mutex & Condition Variable
  pthread_mutex_init(&(pool->lock), NULL);
  pthread_cond_init(&(pool->queue_not_empty), NULL);
  pthread_cond_init(&(pool->queue_empty), NULL);

  // Thread Pool
  pool->workers = (pthread_t *)calloc(num_workers, sizeof(pthread_t)); // worker thread list
  pool->num_workers = num_workers;                                     // number of worker threads

  // Task Queue
  pool->task_queue = (encoding_chunk **)malloc(sizeof(encoding_chunk *) * (1024 * 1024 / 4 + 1)); // 1G divides into 4K files
  pool->head = 0;
  pool->tail = 0;
  pool->size = 0;

  // Result Queue
  pool->result_queue = (encoding_chunk **)malloc(sizeof(encoding_chunk *) * (1024 * 1024 / 4 + 1)); // 1G divides into 4K files
  pool->result_size = 0;
}

// Worker Threads
void *worker_thread(void *threadpool)
{
  threadpool_t *pool = (threadpool_t *)threadpool;
  encoding_chunk *to_encode;

  while (1)
  {
    pthread_mutex_lock(&(pool->lock));

    while (pool->size == 0)
    {
      pthread_cond_wait(&(pool->queue_not_empty), &(pool->lock)); // waiting when no tasks
    }

    // get task
    to_encode = pool->task_queue[pool->head];
    pool->head++;
    pool->size--;

    pthread_mutex_unlock(&(pool->lock));

    encoding_chunk *encoded = encoder(to_encode); // encode the 4KB chunk

    // store in result_queue
    pthread_mutex_lock(&(pool->lock));
    pool->result_queue[to_encode->id] = encoded;
    pool->result_size++;

    // all tasks have finished
    if (pool->result_size == pool->tail)
    {
      pool->result_queue[pool->result_size] = NULL;
      pthread_cond_signal(&(pool->queue_empty));
    }
    pthread_mutex_unlock(&(pool->lock));
  }

  pthread_exit(NULL);
}

int free_pool(threadpool_t *pool)
{
  int i = 0;
  while (pool->task_queue[i] != NULL)
  {
    free(pool->task_queue[i++]);
  }
  free(pool->task_queue);
  /*free(pool->workers);

  pthread_mutex_destroy(&(pool->lock));
  pthread_cond_destroy(&(pool->queue_not_empty));
  pthread_cond_destroy(&(pool->queue_empty));

  free(pool);*/

  return 1;
}
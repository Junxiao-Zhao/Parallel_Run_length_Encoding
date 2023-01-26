#include "global.h"

int free_inputs(char **addr_list, int *size_list, int len)
{
    for (int i = 0; i < len; i++)
    {
        munmap(addr_list[i], size_list[i]);
    }
    free(addr_list);
    free(size_list);

    return 1;
}

// stitch, write, and free the pool->result_queue
int stitch_write_free(encoding_chunk **results)
{
    int i = 0;
    int add_on = 0;
    while (results[i] != NULL)
    {
        results[i]->string_chunk[1] += add_on;
        // last chunk
        if (results[i + 1] == NULL)
        {
            write(1, results[i]->string_chunk, results[i]->size);
        }
        else
        {
            if (results[i + 1]->string_chunk[0] == results[i]->string_chunk[results[i]->size - 2]) // need to stitch
            {
                write(1, results[i]->string_chunk, results[i]->size - 2);
                add_on = results[i]->string_chunk[results[i]->size - 1];
            }
            else
            {
                write(1, results[i]->string_chunk, results[i]->size);
                add_on = 0;
            }
        }
        free(results[i]->string_chunk);
        free(results[i]);
        i++;
    }
    free(results);
    return 0;
}

encoding_chunk *encoder(encoding_chunk *to_encode)
{
    encoding_chunk *encoded = (encoding_chunk *)malloc(sizeof(encoding_chunk));
    encoded->id = to_encode->id;
    encoded->string_chunk = (unsigned char *)malloc(sizeof(unsigned char) * (to_encode->size + 1));

    int pos = 0;
    char pre_c;
    for (int j = 0; j < to_encode->size; j++)
    {
        if (pos != 0 && (to_encode->string_chunk[j] == pre_c)) // same as previous one
            encoded->string_chunk[pos]++;
        else // not the same
        {
            if (pos == 0)
                encoded->string_chunk[pos] = to_encode->string_chunk[j];
            else
                encoded->string_chunk[++pos] = to_encode->string_chunk[j];

            pre_c = to_encode->string_chunk[j];
            encoded->string_chunk[++pos] = 1;
        }
    }

    encoded->size = pos + 1;

    return encoded;
}

int multi_threads(int argc, char **argv, int num_workers)
{
    threadpool_t *pool = init_pool(num_workers);

    int fd;
    struct stat buf_size;
    int size;
    char **addr_list = (char **)malloc(sizeof(char *) * (argc - 2)); // alloc space for files address
    int *size_list = (int *)malloc(sizeof(int) * (argc - 2));        // alloc space for files size

    pthread_mutex_lock(&(pool->lock));

    // add tasks to task queue
    for (int i = 3; i < argc; i++)
    {
        fd = open(argv[i], O_RDONLY, 0644);

        if (fd < 0)
        {
            printf("File not exist");
            break;
        }

        // size of the file
        fstat(fd, &buf_size);
        size = (int)(buf_size.st_size);

        size_list[i - 3] = size;
        addr_list[i - 3] = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
        close(fd);

        int j = 0;
        for (j = 0; j < size; j += 4096)
        {
            pool->task_queue[pool->tail] = (encoding_chunk *)malloc(sizeof(encoding_chunk));
            pool->task_queue[pool->tail]->id = pool->size;
            pool->task_queue[pool->tail]->string_chunk = addr_list[i - 3] + j;

            if (j + 4096 <= size)
            {
                pool->task_queue[pool->tail]->size = 4096;
            }
            else
            {
                pool->task_queue[pool->tail]->size = size - j;
            }
            pool->size++;
            pool->tail++;
        }
    }
    pool->task_queue[pool->tail] = NULL;

    pthread_cond_signal(&(pool->queue_not_empty));

    // create worker threads
    for (int k = 0; k < num_workers; k++)
    {
        pthread_create(&(pool->workers[k]), NULL, worker_thread, (void *)pool);
    }

    // waiting for task finish
    while (pool->size != 0)
    {
        pthread_cond_wait(&(pool->queue_empty), &(pool->lock));
    }

    // output and free
    stitch_write_free(pool->result_queue);
    free_inputs(addr_list, size_list, argc - 2);
    free_pool(pool);
    pthread_mutex_unlock(&(pool->lock));

    return 0;
}
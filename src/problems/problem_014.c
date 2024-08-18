#include "problem_014.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <inttypes.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <pthread.h>

#define PE_014_HIGHEST_START 1000000
#define PE_014_STATUS_BLOCKED -1
#define PE_014_STATUS_CLOSED -2

const int success = EXIT_SUCCESS;
const int failure = EXIT_FAILURE;

pthread_mutex_t data_mutex;
uint32_t parallel_longest_length = 0;
int parallel_longest_start = 0;

typedef struct collatz_run_t {
    int start;
    uint32_t length;
} collatz_run_t;

static void * collatz_sequence(void * arg_in)
{
    collatz_run_t* arg = (collatz_run_t*)arg_in;
    if(arg->start < 0)
    {
        printf("ERROR: invalid start number start = %d", arg->start);
        return (void*)&failure;
    }
    uint64_t n = (uint64_t)(arg->start);
    arg->length = 1;
    while(n != 1)
    {
        if((n % 2) == 0) // even case
        {
            n /= 2;
            (arg->length)++;
        }
        else // odd case
        {
            // for 32-bit signed ints we should be able to go over 715 million
            if(n > (UINT64_MAX/3)) // operation would result in overflow
            {
                printf("ERROR: n too large on run n = %" PRIu64 ", start = %d\n", 
                        n, arg->start);
                return (void *) &failure;
            }
            n = (3*n) + 1;
            (arg->length)++;
        }
    }

    return (void*) &success;
}

#ifdef SERIAL
static int serial_solution(int* out)
{
    int i, serial_longest_start;
    int* ret_ptr;
    uint32_t serial_longest_length = 0;
    collatz_run_t run;
    // with the above collatz_run() method we just iterate through all 1,000,000
    for(i = 1; i < PE_014_HIGHEST_START; i++)
    {
        run.start = i;
        ret_ptr = (int*)collatz_sequence((void*) &run);
        if((*ret_ptr) == EXIT_FAILURE)
        {
            printf("ERROR: serial run %d returned EXIT_FAILURE\n", i);
            return EXIT_FAILURE;
        }
        if(serial_longest_length < run.length)
        {
            serial_longest_start = i;
            serial_longest_length = run.length;
        }
    }

    *out = serial_longest_start;
    return EXIT_SUCCESS;
}
#endif // SERIAL


#ifdef THREAD_POOL
typedef struct tpool_t {
    /* passed as init argumemts */
    int num_threads;

    /* other fields */
    pthread_t* threads;
    int next_start;
    pthread_mutex_t mutex_start;
    _Bool shutdown;
} tpool_t;

typedef struct tpool_order_t {
    void* (*do_work_func)(void* arg);
    int longest_start;
    int next_start;
    uint32_t longest_length;
    tpool_t* tpool_ptr;
} tpool_order_t;

static void* thread_process_collatz(void* arg_in)
{
    tpool_order_t* arg = (tpool_order_t*)arg_in;
    int* ret_ptr;
    collatz_run_t run;

    // do collatz run
    run.length = 0;
    run.start = arg->next_start;
    ret_ptr = collatz_sequence((void*) &run);
    if((*ret_ptr) == EXIT_FAILURE)
    {
        printf("ERROR: thread starting at %d collatz_sequence returned"
                " EXIT_FAILURE\n", arg->next_start);
        return (void*) &failure;
    }

    // store data
    if( (arg->longest_length) < (run.length) )
    {
        (arg->longest_length) = run.length;
        (arg->longest_start) = run.start;
    }

    return (void*)&success;
}

static void* thread_wrapper(void* arg_in)
{
    tpool_order_t* tpool_order_ptr = (tpool_order_t*)arg_in;

    int ret;
    int* ret_ptr;
    tpool_t* tpool_ptr = tpool_order_ptr->tpool_ptr;

    // threads loop waiting for work until shutdown command
    while(true)
    {
        /* lock start mutex */
        ret = pthread_mutex_lock(&(tpool_ptr->mutex_start));
        if(ret == EINVAL)
        {
            fprintf(stderr, "ERROR: mutex_q not initialized correctly\n");
            return (void*)tpool_order_ptr;
        }
        
        /* exit on shutdown regardless of queue size/state */
        if(tpool_ptr->shutdown)
        {
            pthread_mutex_unlock(&(tpool_ptr->mutex_start));
            break;
        }

        /* grab the next available start and increment pool next start */
        if(tpool_ptr->next_start < PE_014_HIGHEST_START)
        {
            tpool_order_ptr->next_start = tpool_ptr->next_start++;
        }
        else
        {
            /* if some thread has already taken all the possible starts, then
             * signal the remaining threads to shutdown and exit */
            tpool_ptr->shutdown = true;
            pthread_mutex_unlock(&(tpool_ptr->mutex_start));
            break;
        }
        
        /* unlock mutex */
        ret = pthread_mutex_unlock(&(tpool_ptr->mutex_start));

        /* if we're here there is work to be done fresh from the buffer */
        int start = tpool_order_ptr->next_start;
        ret_ptr = (int*)(*(tpool_order_ptr->do_work_func))((void*)(tpool_order_ptr));
        if((*ret_ptr) == EXIT_FAILURE)
        {
            fprintf(stderr, "ERROR: Collatz run %d returned EXIT_FAILURE\n", start);
            return (void*)tpool_order_ptr;
        }
    }

    return (void*)tpool_order_ptr;
}


static int tpool_init(tpool_t* tpool_ptr, int num_threads)
{
    int i, ret;
    
    /* pass arguments */
    tpool_ptr->num_threads = num_threads;
    
    /* allocate threads memory region and init */
    tpool_ptr->threads = (pthread_t*)malloc(sizeof(pthread_t)*num_threads);
    if(tpool_ptr->threads == NULL)
    {
        perror("./project euler: tpool_init:");
        return EXIT_FAILURE;
    }
    tpool_order_t* tpool_order_ptrs = (tpool_order_t*)malloc(sizeof(tpool_order_t)*num_threads);
    if(tpool_ptr->threads == NULL)
    {
        perror("./project euler: tpool_init:");
        return EXIT_FAILURE;
    }
    

    /* init mutex and condition variables */
    ret = pthread_mutex_init(&(tpool_ptr->mutex_start), NULL);
    if(ret != 0)
    {
        fprintf(stderr, "ERROR: failed to init thread pool mutex. error = %s", 
                strerror(ret));
        return EXIT_FAILURE;
    }

    /* init the rest */
    tpool_ptr->shutdown = false;
    tpool_ptr->next_start = 1;

    /* create threads and get them going (waiting for data) in the thread
     * wrapper */
    for(i = 0; i < num_threads; i++)
    {
        /* init tpool_order ptr for this id and send it */
        tpool_order_ptrs[i].longest_length = 0;
        tpool_order_ptrs[i].longest_start = 0;
        tpool_order_ptrs[i].next_start = 0;
        tpool_order_ptrs[i].tpool_ptr = tpool_ptr;
        tpool_order_ptrs[i].do_work_func = thread_process_collatz;
        ret = pthread_create(&(tpool_ptr->threads[i]), NULL, thread_wrapper, (void*)&tpool_order_ptrs[i]);
        if(ret != 0)
        {
            printf("ERROR: error when creating thread %d, error = %s\n",
                    i, strerror(ret));
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

static int thread_pool_solution(int* out)
{
    int ret, i;
    long int num_processors;

    // query the number of registered CPUs
    num_processors = sysconf(_SC_NPROCESSORS_CONF);

    // create thread pool
    tpool_t* tpool_ptr;
    /* allocate memory for tpool -- freed later in calling function */
    tpool_ptr = (tpool_t*)malloc(sizeof(tpool_t));
    if(tpool_ptr == NULL)
    {
        perror("./project euler: tpool_init:");
        return EXIT_FAILURE;
    }
    /* work starts here */
    ret = tpool_init(tpool_ptr, (int)(num_processors)); 
    printf("problem_014: created %ld threads for %ld processors\n", 
            num_processors, num_processors);

    /* landing point for return values */
    tpool_order_t** return_values = (tpool_order_t**)malloc(sizeof(tpool_order_t*)*num_processors);

    /* wait for work to complete and then identify longest */
    int longest_start_of_all = 0;
    uint32_t longest_length_of_all = 0;
    for(i = 0; i < tpool_ptr->num_threads; i++)
    {
        if((ret = pthread_join(tpool_ptr->threads[i], (void**)&(return_values[i]))) != 0)
            perror("problem_014: pthread_join failed");
        if(return_values[i]->longest_length > longest_length_of_all)
        {
            longest_length_of_all = return_values[i]->longest_length;
            longest_start_of_all = return_values[i]->longest_start;
        }
    }

    /* free all values */
    free(return_values[0]);     // free the tpool_order_t's malloc'd in init
    free(return_values);        // free the return value pointer array
    free(tpool_ptr->threads);   // free the thread array
    free(tpool_ptr);            // free the threadpool container

    *out = longest_start_of_all;
    return EXIT_SUCCESS;
}
#endif // THREAD_POOL

#ifdef THREAD_POOL_V0
typedef struct work_order_t {
    int start;
    uint32_t length;
    int* longest_start;
    uint32_t* longest_length;
    pthread_mutex_t* mutex_ptr;
} work_order_t;

typedef struct tpool_order_t {
    void* (*do_work_func)(void* arg);
    work_order_t* work_order_ptr;
    struct tpool_order_t* next_tpool_order_ptr;
} tpool_order_t;

typedef struct tpool_t {
    /* passed as init argumemts */
    int num_threads;
    int max_queue_size;
    _Bool do_not_block_when_full;

    /* other fields */
    pthread_t* threads;
    int current_queue_size;
    tpool_order_t* queue_head;
    tpool_order_t* queue_tail;
    pthread_mutex_t mutex_q;
    pthread_mutex_t mutex_count;
    pthread_cond_t cond_q_not_empty;
    pthread_cond_t cond_q_not_full;
    pthread_cond_t cond_q_empty;
    pthread_cond_t cond_count_complete;
    int completed_sequences;
    _Bool queue_closed;
    _Bool shutdown;
} tpool_t;
static void* thread_process_collatz(void* arg_in)
{
    work_order_t* arg = (work_order_t*)arg_in;
    int* ret_ptr;
    int ret;
    collatz_run_t run;

    // do collatz run
    run.length = 0;
    run.start = arg->start;
    ret_ptr = collatz_sequence((void*) &run);
    if((*ret_ptr) == EXIT_FAILURE)
    {
        printf("ERROR: thread starting at %d collatz_sequence returned"
                " EXIT_FAILURE\n", arg->start);
        return (void*) &failure;
    }
    arg->length = run.length;

    // store data using mutex
    ret = pthread_mutex_lock(arg->mutex_ptr);
    if(ret == EINVAL)
    {
        printf("data mutex was not initialized properly\n");
        free(arg);
        return (void*)&failure;
    }

    if( (*(arg->longest_length)) < (arg->length) )
    {
        *(arg->longest_length) = arg->length;
        *(arg->longest_start) = arg->start;
    }

    ret = pthread_mutex_unlock(arg->mutex_ptr);
    if(ret == EINVAL)
    {
        printf("data mutex was not initialized properly\n");
        free(arg);
        return (void*)&failure;
    }
    free(arg);
    return (void*)&success;
}

static void* thread_wrapper(void* arg_in)
{
    tpool_t* tpool_ptr = (tpool_t*)arg_in;

    int ret;
    int* ret_ptr;
    tpool_order_t* tpool_order_ptr;

    // threads loop waiting for work until shutdown command
    while(true)
    {
        /* lock queue mutex */
        ret = pthread_mutex_lock(&(tpool_ptr->mutex_q));
        if(ret == EINVAL)
        {
            printf("ERROR: mutex_q not initialized correctly\n");
            return (void*)&failure;
        }
        
        /* unlock and wait for something to be in the queue */
        while(  (tpool_ptr->current_queue_size == 0) &&
                (!(tpool_ptr->shutdown)))
        {
            pthread_cond_wait(&(tpool_ptr->cond_q_not_empty), 
                    &(tpool_ptr->mutex_q));
        }
        
        /* exit on shutdown regardless of queue size/state */
        if(tpool_ptr->shutdown)
        {
            pthread_mutex_unlock(&(tpool_ptr->mutex_q));
            break;
        }

        /* take the first item of work off the queue */
        tpool_order_ptr = tpool_ptr->queue_head;
        (tpool_ptr->current_queue_size)--;
        if(tpool_ptr->current_queue_size == 0)
        {
            tpool_ptr->queue_head = NULL;
            tpool_ptr->queue_tail = NULL;
            pthread_cond_broadcast(&(tpool_ptr->cond_q_empty));
        }
        else
        {
            tpool_ptr->queue_head = tpool_order_ptr->next_tpool_order_ptr;
        }

        /* if we're in blocking mode when full, broadcast once no longer full */
        if( !(tpool_ptr->do_not_block_when_full) &&
            (tpool_ptr->current_queue_size == (tpool_ptr->max_queue_size - 1)))
        {
            pthread_cond_broadcast(&(tpool_ptr->cond_q_not_full));
        }
        
        /* unlock mutex */
        ret = pthread_mutex_unlock(&(tpool_ptr->mutex_q));

        /* if we're here there is work to be done fresh from the buffer */
        ret_ptr = (int*)(*(tpool_order_ptr->do_work_func))((void*)(tpool_order_ptr->work_order_ptr));
        if((*ret_ptr) == EXIT_FAILURE)
        {
            printf("ERROR: Collatz run %d returned EXIT_FAILURE\n",
                    tpool_order_ptr->work_order_ptr->start);
            return (void*)&failure;
        }

        /* update the completed sequence count */
        ret = pthread_mutex_lock(&(tpool_ptr->mutex_count));
        if(ret == EINVAL)
        {
            printf("ERROR: mutex_q not initialized correctly\n");
            return (void*)&failure;
        }
        tpool_ptr->completed_sequences++;
        if(tpool_ptr->completed_sequences == (PE_014_HIGHEST_START-1))
            pthread_cond_broadcast(&(tpool_ptr->cond_count_complete));
        pthread_mutex_unlock(&(tpool_ptr->mutex_count)); 

        /* free the memory thread has removed from queue and completed */
        free(tpool_order_ptr);
    }

    return (void*)&success;
}


static int tpool_init(tpool_t* tpool_ptr, int num_threads, int max_queue_size, _Bool do_not_block_when_full)
{
    int i, ret;
    
    /* pass arguments */
    tpool_ptr->num_threads = num_threads;
    tpool_ptr->max_queue_size = max_queue_size;
    tpool_ptr->do_not_block_when_full = do_not_block_when_full;
    
    /* allocate threads memory region and init */
    tpool_ptr->threads = (pthread_t*)malloc(sizeof(pthread_t)*num_threads);
    if(tpool_ptr->threads == NULL)
    {
        perror("./project euler: tpool_init:");
        return EXIT_FAILURE;
    }
    

    /* init mutex and condition variables */
    ret = pthread_mutex_init(&(tpool_ptr->mutex_q), NULL);
    if(ret != 0)
    {
        printf("ERROR: failed to init thread pool mutex. error = %s", 
                strerror(ret));
        return EXIT_FAILURE;
    }
    ret = pthread_mutex_init(&(tpool_ptr->mutex_count), NULL);
    if(ret != 0)
    {
        printf("ERROR: failed to init thread pool mutex. error = %s", 
                strerror(ret));
        return EXIT_FAILURE;
    }
    ret = pthread_cond_init(&(tpool_ptr->cond_q_empty), NULL);
    if(ret != 0)
    {
        printf("ERROR: failed to init pool queue condition. error = %s",
                strerror(ret));
        return EXIT_FAILURE;
    }
    ret = pthread_cond_init(&(tpool_ptr->cond_q_not_empty), NULL);
    if(ret != 0)
    {
        printf("ERROR: failed to init pool queue condition. error = %s",
                strerror(ret));
        return EXIT_FAILURE;
    }
    ret = pthread_cond_init(&(tpool_ptr->cond_q_not_full), NULL);
    if(ret != 0)
    {
        printf("ERROR: failed to init pool queue condition. error = %s",
                strerror(ret));
        return EXIT_FAILURE;
    }
    ret = pthread_cond_init(&(tpool_ptr->cond_count_complete), NULL);
    if(ret != 0)
    {
        printf("ERROR: failed to init count complete condition. error = %s",
                strerror(ret));
        return EXIT_FAILURE;
    }

    /* init the rest */
    tpool_ptr->current_queue_size = 0;
    tpool_ptr->queue_head = NULL;
    tpool_ptr->queue_tail = NULL;
    tpool_ptr->queue_closed = false;
    tpool_ptr->shutdown = false;
    tpool_ptr->completed_sequences = 0;

    /* create threads and get them going (waiting for data) in the thread
     * wrapper */
    for(i = 0; i < tpool_ptr->num_threads; i++)
    {
        ret = pthread_create(&(tpool_ptr->threads[i]), NULL, thread_wrapper, (void*)tpool_ptr);
        if(ret != 0)
        {
            printf("ERROR: error when creating thread %d, error = %s\n",
                    i, strerror(ret));
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

static int tpool_add_work(tpool_t* tpool_ptr, void* func_handle, void* arg_in)
{
    tpool_order_t* tpool_order_ptr;
    work_order_t* arg = (work_order_t*)arg_in;
    pthread_mutex_lock(&(tpool_ptr->mutex_q));

    if( (tpool_ptr->current_queue_size == tpool_ptr->max_queue_size) &&
        (tpool_ptr->do_not_block_when_full))
    {
        pthread_mutex_unlock(&(tpool_ptr->mutex_q));
        return PE_014_STATUS_BLOCKED;
    }

    while( (tpool_ptr->current_queue_size == tpool_ptr->max_queue_size) &&
           (!(tpool_ptr->shutdown || tpool_ptr->max_queue_size)))
    {
        pthread_cond_wait(&(tpool_ptr->cond_q_not_full), &(tpool_ptr->mutex_q));
    }

    if(tpool_ptr->shutdown || tpool_ptr->queue_closed)
    {
        pthread_mutex_unlock(&(tpool_ptr->mutex_q));
        return PE_014_STATUS_CLOSED;
    }

    tpool_order_ptr = (tpool_order_t*)malloc(sizeof(tpool_order_t));
    if(tpool_order_ptr == NULL)
    {
        printf("ERROR: not able to allocate memory for run %d\n", arg->start); 
        return EXIT_FAILURE;
    }
    tpool_order_ptr->do_work_func = func_handle;
    tpool_order_ptr->next_tpool_order_ptr = NULL;
    tpool_order_ptr->work_order_ptr = arg;

    if( (tpool_ptr->current_queue_size) == 0)
    {
        tpool_ptr->queue_tail = tpool_ptr->queue_head = tpool_order_ptr;
        tpool_ptr->current_queue_size++;
        pthread_cond_broadcast(&(tpool_ptr->cond_q_not_empty));
    }
    else
    {
        tpool_ptr->queue_tail->next_tpool_order_ptr = tpool_order_ptr;
        tpool_ptr->queue_tail = tpool_order_ptr;
        tpool_ptr->current_queue_size++;
    }
    pthread_mutex_unlock(&(tpool_ptr->mutex_q));
    return EXIT_SUCCESS;
}

static int tpool_free(tpool_t* tpool_ptr, _Bool finish)
{
    int i, ret;

    ret = pthread_mutex_lock(&(tpool_ptr->mutex_q));
    if(ret != 0)
    {
        printf("ERROR: mutex_lock: error = %s\n", strerror(ret));
        return EXIT_FAILURE;
    }

    /* shutdown already in progress */
    if( tpool_ptr->queue_closed || tpool_ptr->shutdown )
    {
        ret = pthread_mutex_unlock(&(tpool_ptr->mutex_q));
        if(ret != 0)
        {
            printf("ERROR: mutex_unlock: error %s\n", strerror(ret));
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    tpool_ptr->queue_closed = true;

    if(finish)
    {
        while(tpool_ptr->current_queue_size != 0)
        {
            ret = pthread_cond_wait(&(tpool_ptr->cond_q_empty), &(tpool_ptr->mutex_q));
            if(ret != 0)
            {
                printf("ERROR: pthread wait cond_q_empty: error = %s\n", 
                        strerror(ret));
                return EXIT_FAILURE;
            }
        }
    }

    tpool_ptr->shutdown = true;

    ret = pthread_mutex_unlock(&(tpool_ptr->mutex_q));
    if(ret != 0)
    {
        printf("ERROR: mutex_unlock err = %s\n", strerror(ret));
        return EXIT_FAILURE;
    }

    ret = pthread_cond_broadcast(&(tpool_ptr->cond_q_not_empty));
    if(ret != 0)
    {
        printf("ERROR: cond broadcast error = %s\n", strerror(ret));
        return EXIT_FAILURE;
    }
    ret = pthread_cond_broadcast(&(tpool_ptr->cond_q_not_full));
    if(ret != 0)
    {
        printf("ERROR: cond broadcast error = %s\n", strerror(ret));
        return EXIT_FAILURE;
    }

    /* now that the threads are signaled to shut down, wait for them to end */
    for(i = 0; i < tpool_ptr->num_threads; i++)
    {
        ret = pthread_join(tpool_ptr->threads[i], NULL);
        if(ret != 0)
        {
            printf("ERROR: pthread join for thread %d, error = %s\n",
                    i, strerror(ret));
            return EXIT_FAILURE;
        }
    }

    free(tpool_ptr->threads);
    free(tpool_ptr);
    return EXIT_SUCCESS;
}

static int thread_pool_solution(int* out)
{
    int start, ret;
    long int num_processors;
    work_order_t* work_order_ptr = NULL;

    // query the number of registered CPUs
    num_processors = sysconf(_SC_NPROCESSORS_CONF);

    // create thread pool
    tpool_t* tpool_ptr;
    /* allocate memory for tpool -- freed later in calling function */
    tpool_ptr = (tpool_t*)malloc(sizeof(tpool_t));
    if(tpool_ptr == NULL)
    {
        perror("./project euler: tpool_init:");
        return EXIT_FAILURE;
    }
    ret = tpool_init(tpool_ptr, (int)(num_processors), 1000000, false); 
    printf("problem_014: created %ld threads for %ld processors\n", 
            num_processors, num_processors);

    // init mutex
    pthread_mutex_init(&data_mutex, NULL);

    // spin off all threads
    for(start = 1; start < PE_014_HIGHEST_START; start++)
    {
        // work_order will be freed by the thread wrapper
        work_order_ptr = (work_order_t*)malloc(sizeof(work_order_t));
        if(work_order_ptr == NULL)
        {
            printf("ERROR: unable to create work order %d\n", start-1);
            return EXIT_FAILURE;
        }
        work_order_ptr->start = start;
        work_order_ptr->length = 0;
        work_order_ptr->longest_length = &parallel_longest_length;
        work_order_ptr->longest_start = &parallel_longest_start;
        work_order_ptr->mutex_ptr = &data_mutex;

        /* this will suspend as needed when queue is full */
        tpool_add_work(tpool_ptr, thread_process_collatz, (void*)work_order_ptr);
    }

    /* wait for completion of queue */
    ret = pthread_mutex_lock(&(tpool_ptr->mutex_count));
    if(ret != 0)
    {
        printf("ERROR: lock mutex_q, error = %s\n", strerror(ret));
        return EXIT_FAILURE;
    }
    ret = pthread_cond_wait(&(tpool_ptr->cond_count_complete), &(tpool_ptr->mutex_count));
    if(ret != 0)
    {
        printf("ERROR: wait cond_q_empty, error = %s\n", strerror(ret));
        return EXIT_FAILURE;
    }
    pthread_mutex_unlock(&(tpool_ptr->mutex_count));

    tpool_free(tpool_ptr, true);

    *out = parallel_longest_start;
    return EXIT_SUCCESS;
}
#endif // THREAD_POOL_V0

int problem_014(problem_solution *ps)
{
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret, ans = 0;

    ps->problem_number = 14U;
    ps->problem_statement = strdup("The following iterative sequence is defined"
            " for the set of positive integers:"
            " [n -> n/2  (n is even)], [n -> 3n + 1 (n is odd)"
            " Using the rule above and starting with 13, we generate the"
            " following sequence:"
            " 13 -> 40 -> 20 -> 10 -> 5 -> 16 -> 8 -> 4 -> 2 -> 1"
            " It can be seen that this sequence (starting at 13 and finishing"
            " at 1) contains 10 terms. Although it has not been proved yet"
            " (Collatz Problem), it is thought that all starting numbers finish"
            " at 1. Which starting number, under one million, produces the" 
            " longest chain? NOTE: Once the chain starts the terms are allowed"
            " to go above one million.");
    
#ifdef SERIAL
    clock_t serial_cpu_start, serial_cpu_end;
    struct timespec serial_abs_start, serial_abs_end;
    double serial_cpu_time_used_ms, serial_abs_time_ms;
    int serial_ans = 0;

    /* start timing */
    serial_cpu_start = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &serial_abs_start)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    /* run serial solution */
    ret = serial_solution(&serial_ans);
    if(ret == EXIT_FAILURE)
        fprintf(stderr, "problem_014: serial_solution returned EXIT_FAILURE\n");

    /* stop timing */
    serial_cpu_end = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &serial_abs_end)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    /* compute times in milliseconds */
    serial_cpu_time_used_ms = 1000.0 * ((double)(serial_cpu_end-serial_cpu_start)) / CLOCKS_PER_SEC;
    if((ret = timespec_subtract(&serial_abs_time_ms, &serial_abs_end, &serial_abs_start)) != EXIT_SUCCESS)
        printf("timespec_subtract returned %d, error = %s\n", ret, strerror(ret));

    /* report timing */
    printf("problem_014: serial_solution cpu_time_ms = %.6f ms\n", serial_cpu_time_used_ms);
    printf("problem_014: serial_solution abs_time_ms = %.6f ms\n", serial_abs_time_ms);

    /* store timing */
    ans = serial_ans;
    ps->cpu_time_ms = serial_abs_time_ms;
#endif //SERIAL

#if defined(THREAD_POOL) || defined(THREAD_POOL_V0)
    clock_t thread_pool_cpu_start, thread_pool_cpu_end; 
    struct timespec thread_pool_abs_start, thread_pool_abs_end;
    double thread_pool_cpu_time_used_ms, thread_pool_abs_time_ms;
    int thread_pool_ans = 0;

    /* start timing */
    thread_pool_cpu_start = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &thread_pool_abs_start)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    /* run thread pool solution */
    if((ret = thread_pool_solution(&thread_pool_ans)) == EXIT_FAILURE)
        fprintf(stderr, "problem_014: thread_pool_solution returned EXIT_FAILURE\n");

    /* stop timing */
    thread_pool_cpu_end = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &thread_pool_abs_end)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    /* compute times in milliseconds */
    thread_pool_cpu_time_used_ms = 1000.0 * ((double)(thread_pool_cpu_end-thread_pool_cpu_start)) / CLOCKS_PER_SEC;
    if((ret = timespec_subtract(&thread_pool_abs_time_ms, &thread_pool_abs_end, &thread_pool_abs_start)) != EXIT_SUCCESS)
        printf("timespec_subtract returned %d, error = %s\n", ret, strerror(ret));
    
    /* report timing */
    printf("problem_014: thread_pool_solution cpu_time_ms = %.6f ms\n", thread_pool_cpu_time_used_ms);
    printf("problem_014: thread_pool_solution abs_time_ms = %.6f ms\n", thread_pool_abs_time_ms);
    
    /* store timing */
    ans = thread_pool_ans;
    ps->cpu_time_ms = thread_pool_abs_time_ms;
#endif //THREAD_POOL

    ret = snprintf(buf, sizeof buf, 
            "The longest Collatz chain starting under 1 million is %d long", 
            ans);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 014:");
        printf("Error: Problem 014: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "%d", ans);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 014:");
        printf("Error: Problem 014: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return EXIT_SUCCESS;
}

#include "problem_014.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <inttypes.h>
#include <limits.h>
#include <pthread.h>

#define PE_014_HIGHEST_START 1000000
#define PE_014_NUM_THREADS (PE_014_HIGHEST_START-1)
#define PE_014_THREADSTACK PTHREAD_STACK_MIN

pthread_mutex_t data_mutex;
uint32_t parallel_longest_length = 0;
int parallel_longest_start = 0;

typedef struct {
    int start;
    uint32_t length;
    int* longest_start;
    uint32_t* longest_length;
    pthread_mutex_t* mutex_ptr;
} work_order_t;

typedef struct {
    int start;
    uint32_t length;
} collatz_run_t;

const int success = EXIT_SUCCESS;
const int failure = EXIT_FAILURE;

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

static void* thread_wrapper(void* arg_in)
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

static int parallel_solution(uint32_t* out)
{
    int start, i, ret;
    int * ret_val;
    work_order_t* work_order_ptr = NULL;

    // allocate all thread memory at once
    pthread_t threads[PE_014_NUM_THREADS];
    
    // set thread stack size
    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, PE_014_THREADSTACK);
    
    // init mutex
    pthread_mutex_init(&data_mutex, NULL);

    // spin off all threads
    for(start = 1; start < PE_014_HIGHEST_START; start++)
    {
        // work_order will be freed by the thread wrapper
        work_order_ptr = (work_order_t*)malloc(sizeof(work_order_t));
        work_order_ptr->start = start;
        work_order_ptr->length = 0;
        work_order_ptr->longest_length = &parallel_longest_length;
        work_order_ptr->longest_start = &parallel_longest_start;
        work_order_ptr->mutex_ptr = &data_mutex;

        // spin the thread off
        ret = pthread_create(&threads[start-1], 
                    &attrs, 
                    thread_wrapper, 
                    (void*)work_order_ptr);
        if(ret == EAGAIN)
        {
            printf("ERROR: Insufficient resources (or a soft limit was reached)"
                    "when attempting to create thread[i] = %d\n", start-1);
            return EXIT_FAILURE;
        }
        else if(ret == EINVAL)
        {
            printf("ERROR: attr has invalid settings\n");
            return EXIT_FAILURE;
        }
        else if(ret == EPERM)
        {
            printf("ERROR: No permission to set the scheduling policy and"
                    " parameters specified in attr.");
            return EXIT_FAILURE;
        }
    }

    // wait for work to be completed
    for(i = 0; i < PE_014_NUM_THREADS; i++)
    {
        ret = pthread_join(threads[i], (void**)&ret_val);
        if(ret == EDEADLK)
        {
            printf("ERROR: deadlock detected when trying to join thread %d\n",i);
            return EXIT_FAILURE;
        }
        else if(ret == EINVAL)
        {
            printf("ERROR: not a joinable thread: i = %d\n", i);
            return EXIT_FAILURE;
        }
        else if(ret == ESRCH)
        {
            printf("ERROR: no thread with given id could be found\n");
            return EXIT_FAILURE;
        }

        if((*ret_val) == EXIT_FAILURE)
        {
            printf("ERROR: Thread returned EXIT_FAILURE, i = %d\n", i);
            return EXIT_FAILURE;
        }
    }

    *out = parallel_longest_start;
    return EXIT_SUCCESS;
}

int problem_014(problem_solution *ps)
{
    clock_t serial_start, serial_end, parallel_start, parallel_end; 
    double serial_cpu_time_used_ms, parallel_cpu_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret, serial_ans = 0, parallel_ans = 0;

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

    serial_start = clock();
    ret = serial_solution(&serial_ans);
    serial_end = clock();
    serial_cpu_time_used_ms = 1000.0 * ((double)(serial_end-serial_start)) 
                                            / CLOCKS_PER_SEC;
    
    parallel_start = clock();
    ret = parallel_solution(&parallel_ans);
    parallel_end = clock();
    parallel_cpu_time_used_ms = 1000.0 * ((double)(parallel_end-parallel_start)) 
                                            / CLOCKS_PER_SEC;

    if(serial_cpu_time_used_ms < parallel_cpu_time_used_ms)
    {
        ps->execution_time_ms = serial_cpu_time_used_ms;
    }
    else
    {
        ps->execution_time_ms = parallel_cpu_time_used_ms;
    }

    ret = snprintf(buf, sizeof buf, 
            "The longest Collatz chain starting under 1 million is %d long", 
            serial_ans);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 014:");
        printf("Error: Problem 014: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "%d", serial_ans);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 014:");
        printf("Error: Problem 014: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return EXIT_SUCCESS;
}

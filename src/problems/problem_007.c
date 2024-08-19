#include "problem_007.h" // export the global function
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <primesieve.h> // we're going to use libprimesieve for this


/* By listing the first six prime numbers: 2, 3, 5, 7, 11 and 13, we can see 
 * that the 6th prime is:
 *      
 *      13
 *
 * What is the 10,001st prime number?
 * */
int problem_007(problem_solution * ps)
{
    clock_t start, end; 
    double cpu_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;

    ps->problem_number = 7U;
    ps->problem_statement = strdup("By listing the first six prime numbers:"
            " 2, 3, 5, 7, 11 and 13, we can see that the 6th prime is: 13"
            " What is the 10,001st prime number?");

    start = clock();
    // Generate the 10,001st prime
    uint64_t n = 10001;
    uint64_t prime = primesieve_nth_prime(n, 0);
    /* end of computation */ 
    end = clock();
    cpu_time_used_ms = 1000.0 * ((double)(end-start)) / CLOCKS_PER_SEC;
    ps->cpu_time_ms = cpu_time_used_ms;
    ps->real_time_ms = 0.0;

    ret = snprintf(buf, sizeof buf, 
            "The %" PRIu64 "th prime is: %" PRIu64, n, prime);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 007:");
        printf("Error: Problem 007: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "%" PRIu64, prime);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 007:");
        printf("Error: Problem 007: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return EXIT_SUCCESS;
}

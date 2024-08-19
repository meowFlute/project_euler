#include "problem_010.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <primesieve.h>

/* Problem 010:
 * The sum of the primes below 10 is: 
 *      2 + 3 + 5 + 7 = 17.
 * Find the sum of all the primes below two million.
 * */
int problem_010(problem_solution *ps)
{
    clock_t start, end; 
    double cpu_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;

    ps->problem_number = 10U;
    ps->problem_statement = strdup("The sum of the primes below 10 is:" 
            " 2 + 3 + 5 + 7 = 17. Find the sum of all the primes below two"
            " million.");

    start = clock();
    /* Solution storage strategy:
     *
     * First we need to decide what we're going to store the primes and the sum
     * in.
     *
     * For the primes, max unsigned 16-bit int (2^16-1) is only 65,535 so it is
     * nowhere near big enough, while max signed 32-bit int is over 2 billion
     * so this would be plenty large for our array
     *
     * Our sum is probably in the hundreds of billions, so max unsigned 32-bit
     * int (2^32 - 1) is very likely too small, while a 64-bit int (2^63 -1)
     * would have 6 digits extra to spare
     * */
    uint64_t begin = 0;
    uint64_t stop = (uint64_t)2000000;
    size_t size, i;
    uint64_t sum = 0;

    /* the primes we can generate with the primesieve library */
    int32_t* primes = (int32_t*) primesieve_generate_primes(begin, stop, &size, INT32_PRIMES);

    /* Solution strategy:
     * given the primes and sum, we just go from index 0 to size and add them
     * up to et the answer */
    for (i = 0; i < size; i++)
    {
        sum += primes[i];
    }

    /* the memory for the array was allocated by libprimesieve, must be freed*/
    primesieve_free(primes);
    end = clock();
    cpu_time_used_ms = 1000.0 * ((double)(end-start)) / CLOCKS_PER_SEC;
    ps->cpu_time_ms = cpu_time_used_ms;
    ps->real_time_ms = 0.0;

    ret = snprintf(buf, sizeof buf, 
            "The sum of the primes under 2,000,000: %" PRIu64, sum);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 010:");
        printf("Error: Problem 010: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "%" PRIu64, sum);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 010:");
        printf("Error: Problem 010: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return EXIT_SUCCESS;
}

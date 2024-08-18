#include "problem_005.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#define NUMBER_OF_PRIMES 46
/* Problem 5
 * 2520 is the smallest number that can be divided by each of the numbers from
 * 1 to 10 without any remainder. 
 *
 * What is the smallest positive number that is
 * evenly divisible by all the numbers from 1 to 20
 * */
uint64_t lcm_of_all_numbers_1_to_n(uint32_t n)
{
    if(n > 200)
    {
        printf("Problem 005: ERROR: Cannot handle numbers greater than 200\n");
        return 0;
    }

    int32_t i;
    double exponent;
    uint64_t solution = 1; // init to 1 so we can just multiply
    // A very simple way to do this is to compute the greatest whole number
    // exponent of each prime that can go into "n" and then multiply all of
    // those together.

    // To generate primes, I'll just list the primes up to 200
    uint32_t primes[NUMBER_OF_PRIMES] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,
        47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,
        149,151,157,163,167,173,179,181,191,193,197,199};

    // Calculate the exponents for all primes less than n and multiply to get
    // the solution
    for(i = 0; i < NUMBER_OF_PRIMES; i++)
    {
        // break from this loop when a prime is larger than n
        if(primes[i] > n)
        {
            break;
        }

        // prime^exponent = n               -- take the log of both sides
        // exponent*log(prime) = log(n)     -- divide by log(prime)
        // exponent = log(n) / log(prime)
        // we evaluate the logs individually for error checking according to
        // the error checking described in man log(3) and man math_error(7)
        double double_n = (double)n;
        double double_primes_i = (double)primes[i];
        if (isnan(double_n) || islessequal(double_n, 0))
        {
            /* Deal with NaN / pole error / domain error */
            printf("ERROR: when n is cast to double, n is %f and log(n) results"
                    "in a NaN / pole / domain error\n", double_n);
            exit(EXIT_FAILURE);
        }
        if (isnan(double_primes_i) || islessequal(double_primes_i, 0))
        {
            /* Deal with NaN / pole error / domain error */
            printf("ERROR: when primes[i] is cast to double, it is %f and"
                    "log(n) results in a NaN / pole / domain error\n", 
                    double_primes_i);
            exit(EXIT_FAILURE);
        }
        // separating the log calls makes it so any additional failure has
        // defined behavior
        double numerator = log(double_n);
        double denominator = log(double_primes_i); 
        exponent = floor(numerator/denominator);
        solution *= (uint64_t)floor(pow((double)primes[i], (double)exponent)); 
    }

    return solution;
}

int problem_005(problem_solution * ps)
{
    clock_t start, end;
    double cpu_time_used_ms;
    ps->problem_number = 5U;
    ps->problem_statement = strdup("2520 is the smallest number that can be"
            " divided by each of the numbers from 1 to 10 without any"
            " remainder. What is the smallest positive number that is evenly"
            " divisible by all the numbers from 1 to 20?");

    /* problem solving execution and timing */
    start = clock();
    uint64_t result = lcm_of_all_numbers_1_to_n((uint32_t)20U);
    end = clock();
    cpu_time_used_ms = 1000.0 * ((double)(end-start)) / CLOCKS_PER_SEC;
    ps->cpu_time_ms = cpu_time_used_ms;
    
    /* store result in problem_solution structure */
    char buf[PE_SOLUTION_BUFFER_LEN]; 
    int ret;
    /* natural language solution */
    ret = snprintf(buf, sizeof buf, 
            "Least common multiple of all numbers 1 to 20: %" PRIu64, result);
    if((ret >= (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 005:");
        printf("Error: Problem 005: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    /* numeric string solution */
    ret = snprintf(buf, sizeof buf, "%" PRIu64, result);
    if((ret >= (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 005:");
        printf("Error: Problem 005: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);

    return EXIT_SUCCESS;
}

#include "problem_003.h"
#include <stdio.h>          // printf
#include <stdlib.h>         // EXIT_SUCCESS
#include <inttypes.h>       // PRIu64
#include <string.h>         // strdup
#include <errno.h>          // errno
#include <time.h>           // clock() utils                        
#include <primesieve.h>     // generate array of primes

/* PROBLEM 3
 * The prime factors of 13195 are 5, 7, 13, 29
 *
 * What is the largest prime factor of the number 600851475143?
 * */

int problem_003(problem_solution * ps)
{
    clock_t start, end;
    double cpu_time_used_ms;
    ps->problem_number = 3U;
    ps->problem_statement = strdup("The prime factors of 13195 are 5, 7, 13, "
            "29. What is the largest prime factor of the number 600851475143?");

    start = clock();
    uint64_t answer = 0;
    uint64_t factored_num;
    uint64_t big_num = (uint64_t)(600851475143UL);
    
    //2^39 is just smaller than our big numberU, and 2^40 is way biggerU, so it
    //has to have less than 39 factorsU, we'll make an array to hold the factors
    //we find and initialize it to zero
    size_t current_factor = 0, i;
    uint32_t factors[39];
    memset(factors, 0, 39*sizeof(uint32_t));

    // Let's just load a big lookup table into memory with all the primes up to
    // the square root of our number (which is 775,146.1)
    size_t size;
    uint32_t* primes = (uint32_t*) primesieve_generate_primes(
                                        (uint64_t)1, 
                                        (uint64_t)775146, 
                                        &size, 
                                        UINT32_PRIMES);
    if((size == 0) || (primes == NULL))
    {    
        //primesieve_generate_primes sets errno on EDOM errors
        perror("project_euler: Problem 003:");
        printf("ERROR: primesieve_generate_primes returned no primes\n");
        return EXIT_FAILURE;
    }
    factored_num = big_num;
    // full factoring loop
    while(factored_num != ((uint64_t)1UL))
    {
        //factor-search loop
        for(i = 0; i < size; i++)
        {
            if((factored_num % primes[i]) == 0)
            {
                // we found a factor!
                // 1) add to the factor list
                // 2) divide factored_num
                // 3) break
                
                /* 1) add to the factor list */
                factors[current_factor] = primes[i];
                current_factor++;

                /* 2) divide the factored_num */
                factored_num /= primes[i];

                /* 3) break */
                break;
            }
        }

        // exited the factor-search loop
        // if primes[i] == 0 then we assume the remaining factor is the answer
        // (a prime larger than the square root, should only be one possible)
        if((primes[i] == 0U) && (current_factor > 0))
        {
            answer = factored_num;
        }
        else if(primes[i] == 0U)
        {
            //this should be an impossible error state
            answer = 0;
            printf("Problem 003: ERROR: Primes list exhausted");
            return EXIT_FAILURE;
        }
        else
        {
            answer = 0;
        }
    }

    //answer = 6857UL; 
    for(i = 0; i <= current_factor; i++)
    {
        if(factors[i] > answer)
            answer = factors[i];
    }

    primesieve_free(primes);
    end = clock();
    cpu_time_used_ms = 1000.0 * ((double)(end-start)) / CLOCKS_PER_SEC;
    ps->cpu_time_ms = cpu_time_used_ms;
    ps->real_time_ms = 0.0;
    
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;
    /* natural language solution */
    ret = snprintf(buf, sizeof buf, 
            "The largest prime factor of %" PRIu64 " is %" PRIu64, 
            big_num, answer);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 003:");
        printf("Error: Problem 003: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    /* numeric string solution */
    ret = snprintf(buf, sizeof buf, "%" PRIu64, answer);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 003:");
        printf("Error: Problem 003: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return EXIT_SUCCESS;
}

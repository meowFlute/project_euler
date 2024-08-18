#include "problem_012.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <inttypes.h>
#include <primesieve.h>

#define PE_012_NUM_PRIMES 10
#define PE_012_INIT_GUESS 5370

// This memory will be allocated once in solution_012(), then used repeatedly
// in count_divisors() until being freed in solution_012()
uint32_t* primes = NULL;

static int count_assumed_divisors(int* out, uint64_t n)
{
    // we assume the following is true:
    // for a prime factorization n = a^p, b^q, c^r, ...:
    // the number of divisors of d = (p+1)*(q+1)*(r+1) ...
    //
    // also assume that the divisors will all occur in the first
    // PE_012_NUM_PRIMES and if they don't it wouldn't have enough anyways
    //
    // also assume the array primes[PE_012_NUM_PRIMES] has already been 
    // initialized
    int i, exponent;
    *out = 1; // init the count to one so it can be multiplied from here
    for(i = 0; i < PE_012_NUM_PRIMES; i++)
    {
        // once we make it past the square root we've exhausted our list
        if((primes[i]) > n)
            break;

        // count the number of times primes[i] goes into n
        exponent = 0; 
        while((n % primes[i]) == 0)
        {
            n /= primes[i];
            exponent++;
        }

        // multiply our count of divisors by the exponent+1
        *out *= (exponent+1);
    }

    if(n > 1)
    {
        // if we get to this point and n > 1 it is because our list of primes
        // wasn't large enough, which we're okay with
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int nth_triangular_num(uint64_t* out, uint32_t n)
{
    // if we limit input to UINT32_MAX-1, then we're guaranteed we won't
    // overflow as long as this operation is 64-bit because 
    // UINT32_MAX = (2^32)-1, and 
    // ((2^32)-1)*((2^32)-2) < ((2^64)-1) = UINT64_MAX
    if(n > (UINT32_MAX - 1))
    {
        printf("ERROR: input to nth_triangular_num greater than limit\n");
        return EXIT_FAILURE;
    }
    // cast here to make it clear to the compiler to do this as a 64-bit 
    // operation to avoid overflows, since the numerator and the result could 
    // be greater than UINT32_MAX but are guaranteed to be less than UINT64_MAX
    *out = ((uint64_t)n)*((uint64_t)(n+1))/((uint64_t)2);
    return EXIT_SUCCESS;
}

int solution_012(uint64_t* out)
{
    /* Solution Strategy:
     * We need to do two main things:
     *  1) generate triangular numbers (1 + 2 + 3 + ... + i)
     *  2) check the number of divisors of triangular numbers (integers)
     * A brute-force way to do this would be to proceed from 1 checking every
     * triangular number's divisors until we hit our target number. 
     *
     * Something maybe a little more efficient would be to generate a bunch of
     * triangular numbers and do a binary search of that group, starting with
     * looking at the largest -- if the largest has less than 500 divisors
     * generate the next group.
     *
     * Wikipedia points out that the nth triangular number is given by n(n+1)/2
     * which means instead of "generating them all one by one" I jump along the
     * set of triangular numbers to arbitrary points.
     *
     * In terms of counting divisors, if the prime factorization 
     * n = a^p + b^q + c^r (etc) is known, then the total number of divisors is 
     * (p+1) * (q+1) * (r+1).
     *
     * We can generate an array of primes to do prime factorization, and from
     * the above formula for counting divisors we know that a number
     * multiplying together the first 9 primes would have 2^9 = 512 divisors
     * 2*3*5*7*11*13*17*19*23 = 223,092,870, but this isn't a triangular
     * number. Does that mean that we can find it with the first 20 primes? 50?
     * We'll leave that as a defined variable and fiddle with it. Generating n
     * primes has low time and memory overhead so starting with the first 100
     * primes won't hurt and it is extremely likely to work.
     *
     * Also, 223,092,870 isn't very big so let's assume it'll fit in a uint64_t
     * for now, we'll use libgmp's mpz_t if we're wrong
     *
     * for an initial guess to speed things up, 14,414,400 is the first number
     * to have more than 500 divisors in the set of integers, so the first in
     * the set of triangular number must be greater than that number
     * (see OEIS A002182). 
     *      n ~= sqrt(14,414,400 * 2) = 5370
     * */

    // array that we'll use for prime factorization that contains the first 
    // PE_012_NUM_PRIMES primes
    primes = (uint32_t*)primesieve_generate_n_primes(
                                        (uint64_t)PE_012_NUM_PRIMES, 
                                        (uint64_t)1, 
                                        UINT32_PRIMES);
    
    // let's find it by just starting and going and see how long that takes
    int d, ret; // number of divisors, return
    uint32_t n; // n in "nth triangular number"
    uint64_t nth_tri; // triangular number
    for(n = PE_012_INIT_GUESS; n < UINT32_MAX; n++)
    {
        // get the nth triangular number
        ret = nth_triangular_num(&nth_tri, n);
        if(ret == EXIT_FAILURE)
        {
            printf("ERROR: n = %" PRIu32 "\n", n);
            return EXIT_FAILURE;
        }
        
        // count divisors
        ret = count_assumed_divisors(&d, nth_tri);
        if(ret == EXIT_FAILURE)
        {
            continue; // this just means it couldn't count enough in the first
                      // PE_012_NUM_PRIMES, and we assume it should be able to
        }

        // break if we found the first one with more than 500 divisors
        if(d > 500)
        {
            *out = nth_tri;
            break;
        }
    }

    primesieve_free(primes);
    
    return EXIT_SUCCESS;
}

int problem_012(problem_solution *ps)
{
    clock_t start, end; 
    double cpu_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;
    uint64_t solution;

    ps->problem_number = 12U;
    ps->problem_statement = strdup("The sequence of triangle numbers is"
            " generated by adding the natural numbers. So the 7th triangle"
            " number would be 1 + 2 + 3 + 4 + 5 + 6 + 7 = 28. The first ten"
            " terms would be: 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, ... Let us"
            " list the factors of the first seven triangle numbers: (1) 1;"
            " (3) 1,3; (6) 1,2,3,6; (10) 1,2,5,10; (15) 1,3,5,15; (21) 1,3,7,21"
            " (28) 1,2,4,7,14,28; We can see that 28 is the first triangle"
            " number to have over five divisors. What is the value of the first"
            " triangle number to have over five hundred divisors?");

    start = clock();

    ret = solution_012(&solution);
    if(ret == EXIT_FAILURE)
    {
        printf("ERROR: Problem 012: solution_012() error\n");
        return EXIT_FAILURE;
    }

    end = clock();
    cpu_time_used_ms = 1000.0 * ((double)(end-start)) / CLOCKS_PER_SEC;
    ps->cpu_time_ms = cpu_time_used_ms;

    ret = snprintf(buf, sizeof buf, 
            "The first triangular number to have 500 divisors is %" PRIu64, 
            solution);
    if((ret >= (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 012:");
        printf("Error: Problem 012: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "%" PRIu64, solution);
    if((ret >= (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 012:");
        printf("Error: Problem 012: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "problem_005.h"

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
    uint64_t solution = 1UL; // init to 1 so we can just multiply
    // A very simple way to do this is to compute the greatest whole number
    // exponent of each prime that can go into "n" and then multiply all of
    // those together.

    // To generate primes, I'll just list the primes up to 200 and terminate
    // the array with a zero
#define NUMBER_OF_PRIMES 46
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
        exponent = floor((( log((double)n) / 
                            log((double)primes[i] ))));
        solution *= (uint64_t)floor(pow((double)primes[i], (double)exponent)); 
    }

    return solution;
}

void problem_005()
{
    printf("Problem 005: Least common multiple of all numbers 1 to 20: %lu\n", 
            lcm_of_all_numbers_1_to_n(20U));
}



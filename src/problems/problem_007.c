#include <stdio.h>
#include <stdint.h>

#include <primesieve.h> // we're going to use libprimesieve for this

#include "problem_007.h" // export the global function

/* By listing the first six prime numbers: 2, 3, 5, 7, 11 and 13, we can see 
 * that the 6th prime is:
 *      
 *      13
 *
 * What is the 10,001st prime number?
 * */
void problem_007()
{
    // Generate the 10,001st prime
    uint64_t n = 10001;
    uint64_t prime = primesieve_nth_prime(n, 0);
    
    printf("Problem 007: %luth prime is: %lu\n", n, prime);
}

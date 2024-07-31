#include <stdio.h>
#include <stdint.h>
#include <primesieve.h>
#include "problem_010.h"

/* Problem 010:
 * The sum of the primes below 10 is: 
 *      2 + 3 + 5 + 7 = 17.
 * Find the sum of all the primes below two million.
 * */
void problem_010()
{
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
    uint64_t start = 0UL;
    uint64_t stop = 2000000UL;
    size_t size, i;
    int64_t sum = 0L;

    /* the primes we can generate with the primesieve library */
    int32_t* primes = (int32_t*) primesieve_generate_primes(start, stop, &size, INT32_PRIMES);

    /* Solution strategy:
     * given the primes and sum, we just go from index 0 to size and add them
     * up to et the answer */
    for (i = 0; i < size; i++)
    {
        sum += primes[i];
    }

    printf("Problem 010: The sum of the primes under 2,000,000:%ld\n", sum);

    /* the memory for the array was allocated by libprimesieve, must be freed*/
    primesieve_free(primes);
}

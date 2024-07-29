#include <stdio.h>

// GNU multiple precision Library
#include <gmp.h>

// global function prototype export
#include "problem_006.h"

/* PROBLEM 006
 * The sum of the squares of the first ten natural numbers is:
 *      1^2 + 2^2 + ... + 10^2 = 385
 *
 * The square of the sum of the first ten natural numbers is:
 *      (1 + 2 + ... + 10)^2 = 55^2 = 3025
 *
 * Hence the difference between the sum of the squares of the first ten natural
 * numbers and the square of the sum is:
 *      3025 - 385 = 2640
 *
 * Find the difference between the sum of the squares of the first one hundred 
 * natural numbers and the square of the sum.
 * */
// for fun we'll make this multipe precision
void sum_of_squares_1_to_n(mpz_t sum, mpz_t n)
{
    mpz_t i, square;
    mpz_init(square); // i to be initialized in for loop

    mpz_set_ui(sum, 0UL); //set sum to zero
    
    for(mpz_init_set_ui(i, 1UL); mpz_cmp(i, n) <= 0; mpz_add_ui(i, i, 1UL))
    {
        mpz_pow_ui(square, i, 2UL);
        mpz_add(sum, sum, square);
    }

    mpz_clears(i, square, NULL);
}

void square_of_sum_1_to_n(mpz_t square, mpz_t n)
{
    mpz_t i, sum;
    mpz_init(sum);

    mpz_set_ui(square, 0UL);

    for(mpz_init_set_ui(i, 1UL); mpz_cmp(i, n) <= 0; mpz_add_ui(i, i, 1UL))
    {
        mpz_add(sum, sum, i);
    }
    mpz_pow_ui(square, sum, 2UL);
}

void problem_006()
{
    mpz_t n, sum, square, difference;
    mpz_inits(n, sum, square, difference, NULL);

    // set n to whatever you want and then calculate the sum and square values
    mpz_set_ui(n, 100UL);
    sum_of_squares_1_to_n(sum, n);
    square_of_sum_1_to_n(square, n);
    mpz_sub(difference, square, sum);
    
    // gmp_printf("Problem 006: The sum of squares from 1 to %Zd: %Zd\n", 
    //         n, sum);
    // gmp_printf("Problem 006: The square of the sum from 1 to %Zd: %Zd\n", 
    //         n, square);
    gmp_printf("Problem 006: The difference of %Zd - %Zd = %Zd\n", 
            square, sum, difference);

    mpz_clears(n, sum, square, difference, NULL);
}

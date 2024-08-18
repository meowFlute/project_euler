#include "problem_006.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <gmp.h>

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
static void sum_of_squares_1_to_n(mpz_t sum, mpz_t n)
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

static void square_of_sum_1_to_n(mpz_t square, mpz_t n)
{
    mpz_t i, sum;
    mpz_init(sum);

    mpz_set_ui(square, 0UL);

    for(mpz_init_set_ui(i, 1UL); mpz_cmp(i, n) <= 0; mpz_add_ui(i, i, 1UL))
    {
        mpz_add(sum, sum, i);
    }
    mpz_pow_ui(square, sum, 2UL);

    mpz_clears(i, sum, NULL);
}

int problem_006(problem_solution * ps)
{
    clock_t start, end; 
    double cpu_time_used_ms;
    /* problem number */
    ps->problem_number = 6U;
    /* problem statement */
    ps->problem_statement = strdup("The sum of the squares of the first ten"
            " natural numbers is: 1^2 + 2^2 + ... + 10^2 = 385. The square of"
            " the sum of the first ten natural numbers is:"
            " (1 + 2 + ... + 10)^2 = 55^2 = 3025. Hence the difference between"
            " the sum of the squares of the first ten natural numbers and the"
            " square of the sum is: 3025 - 385 = 2640. Find the difference"
            " between the sum of the squares of the first one hundred natural"
            " numbers and the square of the sum.");

    /* begin calculation portion */
    start = clock();
    mpz_t n, sum, square, difference;
    mpz_inits(n, sum, square, difference, NULL);

    // set n to whatever you want and then calculate the sum and square values
    mpz_set_ui(n, 100UL);
    sum_of_squares_1_to_n(sum, n);
    square_of_sum_1_to_n(square, n);
    mpz_sub(difference, square, sum);
    
    /* end calculation and calculate CPU time used */
    end = clock();
    cpu_time_used_ms = 1000.0 * ((double)(end-start)) / CLOCKS_PER_SEC;
    ps->cpu_time_ms = cpu_time_used_ms;
    
    /* store solution */
    char buf[PE_SOLUTION_BUFFER_LEN]; 
    int ret;
    /* natural language solution */
    ret = gmp_snprintf(buf, sizeof buf, 
            "The difference of %Zd - %Zd = %Zd", square, sum, difference);
    if((ret >= (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 006:");
        printf("Error: Problem 006: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    /* numeric string solution */
    ret = gmp_snprintf(buf, sizeof buf, "%Zd", difference);
    if((ret >= (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 006:");
        printf("Error: Problem 006: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);

    // make sure to clear all the gmp variables
    mpz_clears(n, sum, square, difference, NULL);
    
    return EXIT_SUCCESS;
}

#include "problem_015.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <gmp.h>

int problem_015(problem_solution *ps)
{
    clock_t start_cpu, end_cpu; 
    struct timespec start_real, end_real;
    double cpu_time_used_ms, real_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;

    ps->problem_number = 15U;
    ps->problem_statement = strdup("Starting in the top left corner of a 2 * 2"
            " grid, and only being able to move to the right and down, there are"
            " exactly 6 routes to the bottom right corner. How many such routes"
            " are there through a 20 * 20 grid?");

    /* start timing */
    start_cpu = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &start_real)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    /* solve problem 015
     * strategy: this problem is theoretically simple to solve. The solution is
     *      40! / (20! * 20!)
     * the main problem with that is those numbers are huge...
     * 40! is over 815 quattuordecillion, so we use gmp (which ships with
     * factorial functions) to compute the solution
     */
    mpz_t fourty_factorial, twenty_factorial, twenty_factorial_product;
    mpz_t quotient, remainder;
    mpz_inits(fourty_factorial, twenty_factorial, twenty_factorial_product, 
            quotient, remainder, NULL);

    mpz_fac_ui(fourty_factorial, 40UL); 
    mpz_fac_ui(twenty_factorial, 20UL); 
    mpz_mul(twenty_factorial_product, twenty_factorial, twenty_factorial); 
    mpz_fdiv_qr(quotient, remainder, fourty_factorial, twenty_factorial_product);
    if(mpz_cmp_ui(remainder, 0) != 0)
        fprintf(stderr, "problem 015: remainder expected to be zero, but wasn't\n");
    char * mallocd_answer = mpz_get_str(NULL, 10, quotient);
    
    /* stop timing */
    end_cpu = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &end_real)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    /* calculate times in milliseconds */
    cpu_time_used_ms = 1000.0 * ((double)(end_cpu-start_cpu)) / CLOCKS_PER_SEC;
    if((ret = timespec_subtract(&real_time_used_ms, &end_real, &start_real)) != RETURN_SUCCESS)
        printf("timespec_subtract returned %d, error = %s\n", ret, strerror(ret));
    

    ps->cpu_time_ms = cpu_time_used_ms;
    ps->real_time_ms = real_time_used_ms;

    ret = snprintf(buf, sizeof buf, 
            "40! / (20! * 20!) = %s", mallocd_answer);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: problem 015:");
        fprintf(stderr, "error: problem 015: snprintf error\n");
        return ENOMEM;
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "%s", mallocd_answer);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 015:");
        fprintf(stderr, "error: problem 015: snprintf error\n");
        return ENOMEM;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);

    mpz_clears(fourty_factorial, twenty_factorial, twenty_factorial_product, 
            quotient, remainder, NULL);
    free(mallocd_answer);
    return RETURN_SUCCESS;
}

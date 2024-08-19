#include "problem_016.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <gmp.h>

int problem_016(problem_solution *ps)
{
    clock_t start_cpu, end_cpu; 
    struct timespec start_real, end_real;
    double cpu_time_used_ms, real_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;

    ps->problem_number = 16U;
    ps->problem_statement = strdup("2^15 = 32768 and the sum of its digits is"
            " 3 + 2 + 7 + 6 + 8 = 26. What is the sum of the digits of the"
            " number 2^1000?");

    /* start timing */
    start_cpu = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &start_real)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    /* solution 16 strategy:
     * This is pretty trivial with GMP
     * We'll: 
     *      1) calculate 2^1000, 
     *      2) get it as a null-terminated string,
     *      3) iterate through all the digit and add them up
     *          - there should be just over 300 digits, so the max it could be
     *          is around 9*300 = 2700, an int should work fine.
     */
    char * solution = NULL;
    int sum = 0;
    int i = 0;

    // get a string containing the digits of 2^1000 using GMP
    mpz_t two_to_one_thou;
    mpz_init(two_to_one_thou);
    mpz_ui_pow_ui(two_to_one_thou, 2UL, 1000UL);
    /* solution MUST be freed after we're done with it (see free below) */
    solution = mpz_get_str(NULL, 10, two_to_one_thou);
    // add up the characters by subtracting '0' from each 
    while(solution[i] != '\0')
    {
        // use the ascii table to index digits 0-9
        sum += solution[i++] - '0';
    }

    /* stop timing */
    end_cpu = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &end_real)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    /* calculate times in milliseconds */
    cpu_time_used_ms = 1000.0 * ((double)(end_cpu-start_cpu)) / CLOCKS_PER_SEC;
    if((ret = timespec_subtract(&real_time_used_ms, &end_real, &start_real)) != RETURN_SUCCESS)
        fprintf(stderr, "timespec_subtract returned %d, error = %s\n", ret, strerror(ret));
    

    ps->cpu_time_ms = cpu_time_used_ms;
    ps->real_time_ms = real_time_used_ms;

    ret = snprintf(buf, sizeof buf, 
            "The sum of the digits of 2^1000: %d", sum);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: problem 016:");
        fprintf(stderr, "error: problem 016: snprintf error\n");
        return ENOMEM;
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "%d", sum);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 016:");
        fprintf(stderr, "error: problem 016: snprintf error\n");
        return ENOMEM;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    if(solution != NULL)
        free(solution);
    return RETURN_SUCCESS;
}

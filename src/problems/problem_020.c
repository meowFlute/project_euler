#include "problem_020.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <gmp.h>

int problem_020(problem_solution *ps)
{
    clock_t start_cpu, end_cpu; 
    struct timespec start_real, end_real;
    double cpu_time_used_ms, real_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;

    ps->problem_number = 20U;
    ps->problem_statement = strdup("n! means n * (n - 1) * ... * 3 * 2 * 1."
            " For example, 10! = 10 * 9 * ... * 3 * 2 * 1 = 3628800, and the sum"
            " of the digits in the number 10! is 3 + 6 + 2 + 8 + 8 + 0 + 0 = 27."
            " Find the sum of the digits in the number 100!.");

    /* start timing */
    start_cpu = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &start_real)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    //Solve problem 020
    mpz_t onehundred_factorial;
    char* str_p_malloc = NULL;
    int str_p_len = 0;
    int i, sum = 0;

    /* get a string with the digits of 100! followed by '\0' */
    mpz_init(onehundred_factorial);
    mpz_fac_ui(onehundred_factorial, 100UL);
    str_p_malloc = mpz_get_str(NULL, 10, onehundred_factorial);
    if(str_p_malloc == NULL)
    {
        fprintf(stderr, "problem 020: error with mpz_get_str(NULL, 10, onehundred_factorial)\n");
        return ENOMEM;
    }
    
    /* add up the digits */
    str_p_len = strlen(str_p_malloc);
    for(i = 0; i < str_p_len; i++)
    {
        sum += str_p_malloc[i] - '0';
    }

    /* free memory */
    free(str_p_malloc);
    mpz_clear(onehundred_factorial);

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
            "the sum of the digits of 100!: %d", sum);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: problem 020:");
        fprintf(stderr, "error: problem 020: snprintf error\n");
        return ENOMEM;
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "%d", sum);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 020:");
        fprintf(stderr, "error: problem 020: snprintf error\n");
        return ENOMEM;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return RETURN_SUCCESS;
}

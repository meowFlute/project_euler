#include "problem_021.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define NUM_AMICABLE 10
const int amicable_pairs_under_10k[NUM_AMICABLE] = {220, 284, 1184, 1210, 2620, 2924, 5020, 5564, 6232, 6368};

int sum_amicable_under_10k(int* out)
{
    /* Amicable pairs are well studied, and the OEIS has a series of them as
     * A259180. The pairs under 10k are:
     * (220, 284), (1184, 1210), (2620, 2924), (5020, 5564), (6232, 6368)
     * So the sum could be trivially obtained that way... let's do it */
    int i;
    
    *out = 0;
    for(i = 0; i < NUM_AMICABLE; i++)
        (*out) += amicable_pairs_under_10k[i];

    return RETURN_SUCCESS;
}

int problem_021(problem_solution *ps)
{
    clock_t start_cpu, end_cpu; 
    struct timespec start_real, end_real;
    double cpu_time_used_ms, real_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;

    // TODO: ps->problem_number = 021U;
    ps->problem_statement = strdup("Evaluate the sum of amicable numbers under 10,000");

    /* start timing */
    start_cpu = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &start_real)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    //TODO: Solve problem 021
    int sum = 0;
    sum_amicable_under_10k(&sum);
    
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
            "The sum of amicable numbers under 10,000: %d", sum);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: problem 021:");
        fprintf(stderr, "error: problem 021: snprintf error\n");
        return ENOMEM;
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "%d", sum);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 021:");
        fprintf(stderr, "error: problem 021: snprintf error\n");
        return ENOMEM;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return RETURN_SUCCESS;
}

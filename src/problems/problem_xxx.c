#include "problem_xxx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

int problem_xxx(problem_solution *ps)
{
    clock_t start_cpu, end_cpu; 
    struct timespec start_real, end_real;
    double cpu_time_used_ms, real_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;

    // TODO: ps->problem_number = xxxU;
    ps->problem_statement = strdup("Insert problem xxx statement here");

    /* start timing */
    start_cpu = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &start_real)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    //TODO: Solve problem xxx
    
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
            "Insert problem xxx natural language solution here");
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: problem xxx:");
        fprintf(stderr, "error: problem xxx: snprintf error\n");
        return ENOMEM;
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "numerical_solution");
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem xxx:");
        fprintf(stderr, "error: problem xxx: snprintf error\n");
        return ENOMEM;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return RETURN_SUCCESS;
}

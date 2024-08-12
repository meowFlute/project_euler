#include "problem_012.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

int problem_012(problem_solution *ps)
{
    clock_t start, end; 
    double cpu_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;

    ps->problem_number = 12U;
    ps->problem_statement = strdup("Insert problem 012 statement here");

    start = clock();
    //TODO: Solve problem 012
    end = clock();
    cpu_time_used_ms = 1000.0 * ((double)(end-start)) / CLOCKS_PER_SEC;
    ps->execution_time_ms = cpu_time_used_ms;

    ret = snprintf(buf, sizeof buf, 
            "Insert problem 012 natural language solution here");
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 012:");
        printf("Error: Problem 012: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "numerical_solution");
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 012:");
        printf("Error: Problem 012: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return EXIT_SUCCESS;
}

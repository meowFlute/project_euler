#include "problem_xxx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

int problem_xxx(problem_solution *ps)
{
    clock_t start, end; 
    double cpu_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;

    // ps->problem_number = xxxU;
    ps->problem_statement = strdup("Insert problem xxx statement here");

    start = clock();
    //TODO: Solve problem xxx
    end = clock();
    cpu_time_used_ms = 1000.0 * ((double)(end-start)) / CLOCKS_PER_SEC;
    ps->execution_time_ms = cpu_time_used_ms;

    ret = snprintf(buf, sizeof buf, 
            "Insert problem xxx natural language solution here");
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem xxx:");
        printf("Error: Problem xxx: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "numerical_solution");
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem xxx:");
        printf("Error: Problem xxx: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return EXIT_SUCCESS;
}

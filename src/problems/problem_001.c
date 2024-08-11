#include "problem_001.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

int sum_of_natural_multiples_below_value(int num_list, int* list, int value);

/* PROBLEM STATEMENT: https://projecteuler.net/problem=1
 *
 * If we list all the natural numbers below 10 that are multiples of 3 or 5, 
 * we get 3, 5, 6, and 9. The sum of these multiples is 23.
 *
 * Find the sum of all the multiples of 3 or 5 below 1000.
 * 
 * */
int problem_001(problem_solution * ps)
{
    clock_t start, end; 
    double cpu_time_used_ms;
    /* problem number */
    ps->problem_number = 1U;
    /* problem statement */
    ps->problem_statement = strdup("If we list all the natural numbers below 10"
            " that are multiples of 3 or 5, we get 3, 5, 6, and 9. "
            "The sum of these multiples is 23. "
            "Find the sume of all the multiples of 3 or 5 below 1000.");

    start = clock();
    const int max_num = 1000;
    int list[2] = { 3, 5 };
    int solution = sum_of_natural_multiples_below_value(
            (int)((sizeof list) / sizeof(int)), list, max_num);
    end = clock();
    cpu_time_used_ms = 1000.0 * ((double)(end-start)) / CLOCKS_PER_SEC;
    ps->execution_time_ms = cpu_time_used_ms;

    /* Buffer length requirements:
     * if we added the number 1000 1000 times, it would be 1000x1000=1000000
     * which is 7 digits. This far exceeds the upper bound of our sum. The
     * natural language string below is 57 characters, so 64 bytes would be an
     * upper limit to our required space. 
     *
     * With a safety factor, we make it 100 bytes */
    /* Update: Made this a #define constant PE_SOLUTION_BUFFER_LEN */
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;
    
    /* natural language solution */
    ret = snprintf(buf, sizeof buf, 
            "The sum of all multiples of 3 or 5 below 1000 is %d",
            solution);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 001:");
        printf("Error: Problem 001: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    /* numeric string solution */
    ret = snprintf(buf, sizeof buf, "%d",solution);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 001:");
        printf("Error: Problem 001: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return EXIT_SUCCESS;
}

int sum_of_natural_multiples_below_value(int num_list, int* list, int value)
{
    int i, j, ret;
    ret = 0;
    for(i = 1; i < value; i++)
    {
        /* for each integer from 1 to value-1 we check if it is a multiple of
         * any of the numbers in our list (which is num_list long) */
        for(j = 0; j < num_list; j++)
        {
            if((i % list[j]) == 0)
            {
                ret += i;
                /* if we have found a multiple we want to break from the j for
                 * loop -- this is the same as doing an or operation with all
                 * of the integers in the list (count each i only once)
                 */
                break;
            }
        }
    }

    return ret;
}

#include "problem_019.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <time.h>

int getdate_error_str(int ret)
{
    switch(ret)
    {
        case 1:
            fprintf(stderr, "The environment variable ‘DATEMSK’ is not defined or null.\n");
            break;
        case 2:
            fprintf(stderr, "The template file denoted by the ‘DATEMSK’ environment variable cannot be opened.\n");
            break;
        case 3:
            fprintf(stderr, "Information about the template file cannot retrieved.\n");
            break;
        case 4:
            fprintf(stderr, "The template file is not a regular file.\n");
            break;
        case 5:
            fprintf(stderr, "An I/O error occurred while reading the template file.\n");
            break;
        case 6:
            fprintf(stderr, "Not enough memory available to execute the function.\n");
            break;
        case 7:
            fprintf(stderr, "The template file contains no matching template.\n");
            break;
        case 8:
            fprintf(stderr, "The input date is invalid, but would match a template otherwise.\n");
            break;
    }
    return EINVAL;
}

/* count how many sundays fall on the first of the month between two dates */
int count_first_sundays(int* out, int start_month, int start_year,
                                  int end_month,   int end_year)
{
    int y, m, ret;
    char buf[128];
    struct tm date_tm;
    int sum = 0;

    if((start_month < 1) || (start_month > 12))
    {
        fprintf(stderr, "problem 019: invalid start month\n");
        return EINVAL;
    }
    if((start_year < 0) || (start_year >= end_year))
    {
        fprintf(stderr, "problem 019: invalid start year\n");
        return EINVAL;
    }
    if((end_month < 1) || (end_month > 12))
    {
        fprintf(stderr, "problem_019: invalid end month\n");
        return EINVAL;
    }
    if((ret = setenv("DATEMSK", "resources/datemsk.txt", true)) != 0)
    {
        perror("problem 019: setenv failed");
        return errno;
    }

    for(y = start_year; y <= end_year; y++)
    {
        for(m = start_month; m <= 12; m++)
        {
            //get a struct tm of the first sunday of the given month/year combo
            // corresponds to "%a %m %Y" in resources/datemsk.txt
            ret = snprintf(buf, sizeof buf, "Sun %d %d", m, y);
            if((ret >= (int)(sizeof buf)) || (ret < 0))
                fprintf(stderr, "problem 019: snprintf of date returned bad value ret=%d\n", ret);
            if((ret = getdate_r(buf, &date_tm)) != 0)
                return getdate_error_str(ret);
            if(date_tm.tm_mday == 1)
                sum++;
        }
    }
    *out = sum;
    return RETURN_SUCCESS;
}

int problem_019(problem_solution *ps)
{
    clock_t start_cpu, end_cpu; 
    struct timespec start_real, end_real;
    double cpu_time_used_ms, real_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;

    ps->problem_number = 19U;
    ps->problem_statement = strdup("How many Sundays fell on the first of the"
            "month during the twentieth century (1 Jan 1901 to 31 Dec 2000)?");

    /* start timing */
    start_cpu = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &start_real)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    //Solve problem 019
    int ans;
    if((ret = count_first_sundays(&ans, 1, 1901, 12, 2000)) != RETURN_SUCCESS)
        fprintf(stderr, "problem 019: count_first_sundays returned %d, %s\n", ret, strerror(ret));
    
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
            "number of sundays that fell on the first of the month in the 20th century: %d", ans);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: problem 019:");
        fprintf(stderr, "error: problem 019: snprintf error\n");
        return ENOMEM;
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "%d", ans);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 019:");
        fprintf(stderr, "error: problem 019: snprintf error\n");
        return ENOMEM;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return RETURN_SUCCESS;
}

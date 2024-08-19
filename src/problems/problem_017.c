#include "problem_017.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

int problem_017(problem_solution *ps)
{
    clock_t start_cpu, end_cpu; 
    struct timespec start_real, end_real;
    double cpu_time_used_ms, real_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;

    ps->problem_number = 17U;
    ps->problem_statement = strdup("If the numbers 1 to 5 are written out in"
            " words: one, two, three, four, five, then there are"
            " 3 + 3 + 5 + 4 + 4 = 19 letters used in total. If all the numbers"
            " from 1 to 1000 (one thousand) inclusive were written out in"
            " words, how many letters would be used?"
            " NOTE: Do not count spaces or hyphens. For example, 342"
            " (three hundred and forty-two) contains 23 letters and"
            " 115 (one hundred and fifteen) contains 20 letters."
            " The use of \"and\" when writing out numbers is in compliance with"
            " British usage.");

    /* start timing */
    start_cpu = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &start_real)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    // Solve problem 017
    int ones, tens, hundreds;
    int sum = 0;
    for(hundreds = 0; hundreds < 10; hundreds++)
    {
        for(tens = 0; tens < 10; tens++)
        {
            for(ones = 0; ones < 10; ones++)
            {
                switch(hundreds)
                {
                    case 0:
                        // ""
                        sum += 0;
                        break;
                    case 1:
                        // "onehundred"
                        sum += 10;
                        break;
                    case 2:
                        // "twohundred"
                        sum += 10;
                        break;
                    case 3:
                        // "threehundred"
                        sum += 12;
                        break;
                    case 4:
                        // "forhundred"
                        sum += 10;
                        break;
                    case 5:
                        // "fivehundred"
                        sum += 11;
                        break;
                    case 6:
                        // "sixhundred"
                        sum += 10;
                        break;
                    case 7:
                        // "sevenhundred"
                        sum += 12;
                        break;
                    case 8:
                        // "eighthundred"
                        sum += 12;
                        break;
                    case 9:
                        // "ninehundred"
                        sum += 11;
                        break;
                    default:
                        fprintf(stderr, "unexpected hundreds %d\n", hundreds);
                        break;
                }

                if((hundreds > 0) &&((tens > 0) || (ones > 0)))
                    sum += 3; //and

                if((tens == 1) && (ones > 0))
                {
                    // catch the odd-ball cases of eleven, twelve, and so on
                    switch(ones)
                    {
                        case 1:
                            // "eleven"
                            sum += 6;
                            break;
                        case 2:
                            // "twelve"
                            sum += 6;
                            break;
                        case 3:
                            // "thirteen"
                            sum += 8;
                            break;
                        case 4:
                            // "fourteen"
                            sum += 8;
                            break;
                        case 5:
                            // "fifteen"
                            sum += 7;
                            break;
                        case 6:
                            // "sixteen"
                            sum += 7;
                            break;
                        case 7:
                            // "seventeen"
                            sum += 9;
                            break;
                        case 8:
                            // "eighteen"
                            sum += 8;
                            break;
                        case 9:
                            // "nineteen"
                            sum += 8;
                            break;
                        default:
                            fprintf(stderr, "unexpected teens %d\n", ones);
                            break;
                    }
                }
                else
                {
                    switch(tens)
                    {
                        case 0:
                            //""
                            sum += 0;
                            break;
                        case 1:
                            //ten
                            sum += 3;
                            break;
                        case 2:
                            //twenty
                            sum += 6;
                            break;
                        case 3:
                            //thirty
                            sum += 6;
                            break;
                        case 4:
                            //fourty
                            sum += 6;
                            break;
                        case 5:
                            //fifty
                            sum += 5;
                            break;
                        case 6:
                            //sixty
                            sum += 5;
                            break;
                        case 7:
                            //seventy
                            sum += 7;
                            break;
                        case 8:
                            //eighty
                            sum += 6;
                            break;
                        case 9:
                            //ninety
                            sum += 6;
                            break;
                        default:
                            fprintf(stderr, "unexpected tens %d\n", tens);
                            break;
                    }
                    switch(ones)
                    {
                        case 0:
                            //""
                            sum += 0;
                            break;
                        case 1:
                            //one
                            sum += 3;
                            break;
                        case 2:
                            //two
                            sum += 3;
                            break;
                        case 3:
                            //three
                            sum += 5;
                            break;
                        case 4:
                            //four
                            sum += 4;
                            break;
                        case 5:
                            //five
                            sum += 4;
                            break;
                        case 6:
                            //six
                            sum += 3;
                            break;
                        case 7:
                            //seven
                            sum += 5;
                            break;
                        case 8:
                            //eight
                            sum += 5;
                            break;
                        case 9:
                            //nine
                            sum += 4;
                            break;
                        default:
                            fprintf(stderr, "unexpected one %d\n", ones);
                            break;
                    }
                }
            }
        }
    }
    //onethousand
    sum += 11;

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
            "The sum of all the letters of one to one thousand: %d", sum);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: problem 017:");
        fprintf(stderr, "error: problem 017: snprintf error\n");
        return ENOMEM;
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "%d", sum);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 017:");
        fprintf(stderr, "error: problem 017: snprintf error\n");
        return ENOMEM;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return RETURN_SUCCESS;
}

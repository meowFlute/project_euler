#include "problem_001.h"
#include <stdio.h>

/* TODO: come back and make this use large unsigned stuff to make it robust */
int sum_of_natural_multiples_below_value(int num_list, int* list, int value);

/* PROBLEM STATEMENT: https://projecteuler.net/problem=1
 *
 * If we list all the natural numbers below 10 that are multiples of 3 or 5, 
 * we get 3, 5, 6, and 9. The sum of these multiples is 23.
 *
 * Find the sum of all the multiples of 3 or 5 below 1000.
 * 
 * */
void problem_001(void)
{
#define NUM_IN_LIST 2
#define MAX_NUM 1000
    int list[NUM_IN_LIST] = { 3, 5 };

    printf("The answer to problem 001 is %d!\n", 
            sum_of_natural_multiples_below_value(NUM_IN_LIST, list, MAX_NUM));
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

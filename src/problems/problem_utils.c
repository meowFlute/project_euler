#include "problem_utils.h"
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define ONE_BILLION 1000000000

/* Subtract the ‘struct timespec’ values X and Y,
 *      storing the result in RESULT_MS.
 *      Return EINVAL if passed a null pointer, EXIT_SUCCESS otherwise */
int timespec_subtract (double *result_ms, struct timespec *x, struct timespec *y)
{
    struct timespec result;

    /* NULL for any argument pointer is a failure of error code EINVAL */
    if((x == NULL) || (y == NULL) || (result_ms == NULL))
        return EINVAL;

    /* Perform the carry for the later subtraction by updating Y. */
    if (x->tv_nsec < y->tv_nsec) 
    {
      int sec = (y->tv_nsec - x->tv_nsec) / ONE_BILLION + 1;
      y->tv_nsec -= ONE_BILLION * sec;
      y->tv_sec += sec;
    }
    if (x->tv_nsec - y->tv_nsec > ONE_BILLION) 
    {
      int sec = (x->tv_nsec - y->tv_nsec) / ONE_BILLION;
      y->tv_nsec += ONE_BILLION * sec;
      y->tv_sec -= sec;
    }

    /* Compute the time delta
     * ‘result.tv_nsec’ is guaranteed to be positive. */
    result.tv_sec = x->tv_sec - y->tv_sec;
    result.tv_nsec = x->tv_nsec - y->tv_nsec;

    /* convert both to milliseconds */
    *result_ms = (result.tv_sec*1000) + (((double)(result.tv_nsec))/1000000.0);

    return EXIT_SUCCESS;
}

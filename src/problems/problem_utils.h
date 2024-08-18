#ifndef _PROBLEM_UTILS_H
#define _PROBLEM_UTILS_H

/* struct timespec is defined in <time.h> */
#include <time.h>

/* time-related utility functions */
int timespec_subtract(double *result_ms, struct timespec *x, struct timespec *y);

#endif //_PROBLEM_UTILS_H

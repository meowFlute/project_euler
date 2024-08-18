#ifndef _PROJECT_EULER
#define _PROJECT_EULER

/* program version tracking and reporting constants */
/* HIGHEST_PROBLEM_COMPLETED also defines problem array lengths and iterator
 * stopping points */
#define MAIN_VERSION                3
#define HIGHEST_PROBLEM_COMPLETED   14
#define SUB_VERSION                 0

#include "display_results.h"

#include "problems/problem_types.h"
#include "problems/problem_utils.h"
#include "problems/problem_001.h"
#include "problems/problem_002.h"
#include "problems/problem_003.h"
#include "problems/problem_004.h"
#include "problems/problem_005.h"
#include "problems/problem_006.h"
#include "problems/problem_007.h"
#include "problems/problem_008.h"
#include "problems/problem_009.h"
#include "problems/problem_010.h"
#include "problems/problem_011.h"
#include "problems/problem_012.h"
#include "problems/problem_013.h"
#include "problems/problem_014.h"

/* global variables that need to get from main.c to display_results.c */
extern _Bool problems[HIGHEST_PROBLEM_COMPLETED];
extern _Bool report_time;
extern _Bool numeric;
extern _Bool natural_language;
extern _Bool problem_statement;
extern _Bool tabulated;
extern problem_solution solution_arr[HIGHEST_PROBLEM_COMPLETED];

#endif // _PROJECT_EULER

#ifndef _PROBLEM_TYPES_H
#define _PROBLEM_TYPES_H

/* The main things we may want back from a problem:
 *  -   problem number
 *  -   problem statement
 *  -   numerical answer (only what you'd enter into the textbox on
 *      projecteuler.net)
 *          > because of the range of answers this probably needs to be a
 *          string so we can get any number of digits signed or not from any
 *          data type
 *  -   natural language answer (e.g. "the largest 4-term product in the grid
 *  is <numerical answer>)
 * */
typedef struct problem_solution {
    unsigned int problem_number;
    char * problem_statement;
    char * numerical_solution;
    char * natural_language_solution;
    double execution_time_ms;
} problem_solution;

void free_problem_solution_strings(problem_solution * ps);

#endif //_PROBLEM_TYPES_H

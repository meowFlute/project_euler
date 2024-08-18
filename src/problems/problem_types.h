#ifndef _PROBLEM_TYPES_H
#define _PROBLEM_TYPES_H

// I'm going to use errno for my return statuses wherever possible, but I don't
// like how "return 0" looks, and EXIT_SUCCESS isn't 'guaranteed' to be zero
#define RETURN_SUCCESS 0

// Project Euler Solution Buffer Length will be common across all solutions so
// that I can use strnlen and others to varify 
#define PE_SOLUTION_BUFFER_LEN 100

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
    double cpu_time_ms;
    double real_time_ms;
} problem_solution;

void free_problem_solution_strings(problem_solution * ps);

#endif //_PROBLEM_TYPES_H

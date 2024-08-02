#include <stdlib.h>

#include "problem_types.h"

void free_problem_solution_strings(problem_solution * ps)
{
    /* free shouldn't touch errno and has no return type, 
     * so no error checking here */

    /* we use strdup to allocate memory for these strings, so we must free them
     * */
    free(ps->natural_language_solution);
    free(ps->numerical_solution);
    free(ps->problem_statement);
}

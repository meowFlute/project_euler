/* LIBRARY HEADER FILES */
#include <argp.h>       // argp functions
#include <stdbool.h>    // false
#include <stdlib.h>     // EXIT_SUCCESS
#include <string.h>     // strtok
#include <errno.h>      // errno
#include <error.h>      // error

/* LOCAL HEADER FILES */
#include "project_euler.h"

#define MAIN_VERSION                1
#define HIGHEST_PROBLEM_COMPLETED   10
#define SUB_VERSION                 1

// macros to make DEF2STR(HIGHEST_PROBLEM_COMPLETED) = "8" (or whatever number)
// used below to make it so argp_program_version 
#define DEF2STRX(x) #x
#define DEF2STR(x) DEF2STRX(x)

/* array of problem run flags */
_Bool problems[HIGHEST_PROBLEM_COMPLETED] = {false,};
_Bool argument_encountered = false;

/* array of problem function pointers */
void (*problem_func_ptrs[HIGHEST_PROBLEM_COMPLETED]) (void) = {
    problem_001,
    problem_002,
    problem_003,
    problem_004,
    problem_005,
    problem_006,
    problem_007,
    problem_008,
    problem_009,
    problem_010
};

/* argp globals */
const char * argp_program_version = "project_euler version "
        DEF2STR(MAIN_VERSION) "." 
        DEF2STR(HIGHEST_PROBLEM_COMPLETED) "."
        DEF2STR(SUB_VERSION);
const char * argp_program_bug_address = "<mscottchristensen@gmail.com>";
const char doc[] = "project_euler -- a runnable set of solutions to the "
        "problems found on projecteuler.net. The version number denotes "
        "<main version number>.<max problem completed>.<sub version number>. "
        "This means that you can't run a problem greater than whatever is in "
        "the <max problem complete> field of the version number";

static error_t project_euler_parser(int key, char * arg, 
        struct argp_state * state);

static struct argp_option options[] = {
        {"problems", 'p', "CSV_PROBS", 0,
            "comma-separated (no spaces) integers representing problems to run."
            "e.g. 1,5,23", 0},
        {"all", 'a', 0, 0,
            "run all of the problems", 0},
        { 0 }
    };

static struct argp parser = { options, project_euler_parser, 0, doc, 0, 0, 0 };

static error_t project_euler_parser(int key, char * arg, 
        struct argp_state * state)
{
    if(key == ARGP_KEY_END)
    {
        if(!argument_encountered)
        {
            argp_error(state, "at least one argument is required");
        }
    }
    if(key == 'a')
    {
        argument_encountered = true;
        int i;
        for(i = 0; i < HIGHEST_PROBLEM_COMPLETED; i++)
        {
            problems[i] = true;
        }
    }
    if(key == 'p')
    {
        argument_encountered = true;
        char * arg_copy = strdup(arg);
        if(arg_copy == NULL)
            error(EXIT_FAILURE, errno, "arg_copy malloc failed");

        char * ptr = strsep(&arg_copy, ",");
        if(ptr == NULL)
            argp_error(state, "argument required for --problems");
        while(ptr != NULL)
        {
            int sub_arg = atoi(ptr);
            sub_arg--; // the array of flags is indexed to zero for problem 1
            if(sub_arg > 0)
            {
                problems[sub_arg] = true;
            }
            else // zero means an error
            {
                argp_error(state, "Do not pass 0 as a problem number.");
            }
            ptr = strsep(&arg_copy, ",");
        }

        free(arg_copy);
    }

    return 0;
}

int main(int argc, char * argv[])
{
    int i;

    /* Run the above arg parser */
    argp_parse(&parser, argc, argv, 0, 0, 0);

    for(i = 0; i < HIGHEST_PROBLEM_COMPLETED; i++)
    {
        if(problems[i])
            problem_func_ptrs[i](); 
    }

    return 0;
}

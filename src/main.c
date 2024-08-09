/* LIBRARY HEADER FILES */
#include <argp.h>       // argp functions
#include <stdbool.h>    // false
#include <stdlib.h>     // EXIT_SUCCESS
#include <string.h>     // strtok
#include <errno.h>      // errno
#include <error.h>      // error
#include <time.h>       // cpu timing
#include <math.h>       // log
#ifdef __linux__
#include <sys/ioctl.h>  // ioctl
#include <unistd.h>     // STDOUT_FILENO
#endif

/* LOCAL HEADER FILES */
#include "project_euler.h"

#define MAIN_VERSION                2
#define HIGHEST_PROBLEM_COMPLETED   11
#define SUB_VERSION                 1

// macros to make DEF2STR(HIGHEST_PROBLEM_COMPLETED) = "8" (or whatever number)
// used below to make it so argp_program_version 
#define DEF2STRX(x) #x
#define DEF2STR(x) DEF2STRX(x)

/* array of problem run flags */
_Bool problems[HIGHEST_PROBLEM_COMPLETED] = {false,};
_Bool argument_encountered = false;
_Bool report_time = false;
_Bool numeric = true; //true by default, option sets false
_Bool natural_language = false;
_Bool problem_statement = false;
_Bool tabulated = false;
clock_t cpu_time_start, cpu_time_end;
double cpu_time_used;

/* array of problem function pointers */
int (*problem_func_ptrs[HIGHEST_PROBLEM_COMPLETED]) (problem_solution * ps) = {
    problem_001,
    problem_002,
    problem_003,
    problem_004,
    problem_005,
    problem_006,
    problem_007,
    problem_008,
    problem_009,
    problem_010,
    problem_011
};

problem_solution solution_arr[HIGHEST_PROBLEM_COMPLETED];

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
        {"time", 't', 0, 0,
            "track and report CPU time of each problem solution", 0},
        {"no_numeric_only", 'n', 0, 0,
            "report only numbers for solutions", 0},
        {"tabulated", 'z', 0, 0,
            "report solutions in a table", 0},
        {"problem_statement", 's', 0, 0,
            "include problem statements in the results", 0},
        {"natural_language", 'x', 0, 0,
            "report full-length text solutions in natural language", 0},
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
            argp_error(state, "at least one argument is required "
                    "(-t,--time doesn't count)");
        }
    }
    if(key == 'n')
        numeric = false;
    if(key == 's')
        problem_statement = true;
    if(key == 'x')
        natural_language = true;
    if(key == 'z')
        tabulated = true;
    if(key == 't')
        report_time = true;
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
            if(sub_arg > 0)
            {
                sub_arg--; // the array of flags is indexed to zero for problem 1
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

typedef struct tabulated_widths{
    unsigned short total;
    unsigned short problem_num;
    unsigned short exe_time_width;
    unsigned short exe_time_precision;
    unsigned short numerical_solution;
    unsigned short problem_statement;
    unsigned short natural_language;
} tabulated_widths;

void print_width(int width, char c)
{
    int i;
    for(i = 0; i < width; i++)
    {
        putchar(c);
    }
    putchar('\n');
}

int report_results_tabulated_single_line(tabulated_widths tw)
{
    /* There are two cases where we'd want to do this:
     *  1) not a tty
     *  2) not a wide enough tty
     * In those cases, we'll simply assume infinite width. We also assume all
     * strings have no newline characters.
     * */
    int i;

    // since we're reporting all of these on one line, we need to set the
    // length of the strings that would normally be wrapped.
    tw.natural_language = strlen("natural language solution");
    tw.problem_statement = strlen("problem statement");
    for(i = 0; i < HIGHEST_PROBLEM_COMPLETED; i++)
    {
        if(problems[i])
        {
            unsigned short tmp_nat, tmp_prob;
            tmp_nat = (unsigned short)strlen(solution_arr[i].natural_language_solution);
            tmp_prob = (unsigned short)strlen(solution_arr[i].problem_statement);
            
            if(tmp_nat > tw.natural_language)
                tw.natural_language = tmp_nat;
            
            if(tmp_prob > tw.problem_statement)
                tw.problem_statement = tmp_prob;
        }
    }

    tw.total = 0U;
    printf("| %-*s ", tw.problem_num, "num");
    tw.total += tw.problem_num + 3U;
    if(report_time)
    {
        printf("| %-*s ", 
                tw.exe_time_width + tw.exe_time_precision + 4U, "exe time");
        tw.total += tw.exe_time_precision + tw.exe_time_width + 7U;
    }
    if(numeric)
    {
        printf("| %-*s ", tw.numerical_solution, "answer");
        tw.total += tw.numerical_solution + 3U;
    }
    if(natural_language)
    {
        printf("| %-*s ", tw.natural_language, "natural language solution");
        tw.total += tw.natural_language + 3U;
    }
    if(problem_statement)
    {
        printf("| %-*s ", tw.problem_statement, "problem statement");
        tw.total += tw.problem_statement + 3U;
    }
    printf("|\n");
    tw.total += 1U;

    print_width(tw.total, '=');

    for(i = 0; i < HIGHEST_PROBLEM_COMPLETED; i++)
    {
        if(problems[i])
        {
            printf("| %0*u ", tw.problem_num, solution_arr[i].problem_number);
            if(report_time)
                printf("| %*.*f ms ", 
                        tw.exe_time_width, 
                        tw.exe_time_precision,
                        solution_arr[i].execution_time_ms);
            if(numeric)
                printf("| %*s ", 
                        tw.numerical_solution, 
                        solution_arr[i].numerical_solution);
            if(natural_language)
                printf("| %-*s ", 
                        tw.natural_language, 
                        solution_arr[i].natural_language_solution);
            if(problem_statement)
                printf("| %-*s ", 
                        tw.problem_statement, 
                        solution_arr[i].problem_statement);
            printf("|\n");
            print_width(tw.total, '-');
        }
    }
    return EXIT_SUCCESS;
}

int report_results(void)
{
    int i;
    /* we need to handle a few potential output styles:
     *      - tabulated:
     *          > possible columns: 
     *              1) problem number
     *              2) problem statements (optional)
     *              3) natural language problem solutions (optional)
     *              4) numeric only problem solutions (default optional)
     *              5) execution time (optional)
     *      - non-tabulated:
     *          > possible reported lines:
     *              same 5 as above
     */

    /* tabulated output */
    if(tabulated)
    {
        /* I'm thinking we'll have to implement different methods of getting
         * console width and tabulating the output. I'll start with linux and
         * then see if I can add windows. A stretch goal would be to get a mac
         * version to compile and test as a github action without ever
         * owning a mac, but I don't really care if that happens
         *
         * We'll keep them separate with compiler macros so we don't get into
         * too much trouble */

        /* calculate column widths for all platforms */
        /* We need to subdivide the max width by each field we're going to
         * use. We'll do the single-line columns first to figure out how
         * much width we have remaining */

        tabulated_widths tw;
        _Bool single_line_print = false;
        unsigned short tmp_time_width, tmp_numerical_width;
        tw.total = 0U;

        /* fixed width columns:
         *      - problem number
         *      - time elapsed (optional) 
         *      - numeric_only solution (optional, default)
         * */ 
        tw.problem_num = 3U;
        tw.exe_time_precision = 6U;
        tw.exe_time_width = 1U;
        tw.numerical_solution = 6U; // "answer" is the min column width
        for(i = 0; i < HIGHEST_PROBLEM_COMPLETED; i++)
        {
            if(problems[i])
            {
                if(report_time)
                {
                    if(solution_arr[i].execution_time_ms > 1.0)
                    {
                        tmp_time_width = (unsigned short)floor(
                                log10(solution_arr[i].execution_time_ms));
                        if(tmp_time_width > tw.exe_time_width)
                            tw.exe_time_width = tmp_time_width;
                    }
                }
                else
                {
                    tw.exe_time_width = 0U;
                    tw.exe_time_precision = 0U;
                }

                if(numeric) 
                {
                    tmp_numerical_width = (unsigned short)strnlen(
                            solution_arr[i].numerical_solution, 
                            PE_SOLUTION_BUFFER_LEN);
                    if(tmp_numerical_width == PE_SOLUTION_BUFFER_LEN)
                    {
                        /* this is an error condition where '\0' was never
                         * found in the string */
                        printf("ERROR: problem %d numerical_solution string"
                                " does not contain a null termination char"
                                "\n", i);
                        return EXIT_FAILURE;
                    }
                    else
                    {
                        if(tmp_numerical_width > tw.numerical_solution)
                            tw.numerical_solution = tmp_numerical_width;
                    }
                }
                else
                {
                    tw.numerical_solution = 0U;
                }
            }
        }

        /* calculate how much width we have left for the wrapped columns */
        tw.total += tw.problem_num + 3U; // adding 3 for spaces and column
        if(report_time)
            tw.total += tw.exe_time_precision + tw.exe_time_width
                + 7U; // adding 6 for spaces, decimal, "ms", and right '|'
        if(numeric)
            tw.total += tw.numerical_solution + 3U; // spaces, '|' 
        tw.total += 1U; // for the final column

#if __linux__

        /* w.ws_col contains the column width we need to know to keep
         * formatting nice after we use ioctl to check the term */
        if(isatty(STDOUT_FILENO))
        {
            /* STDOUT is a tty */
            struct winsize w;

            /* use ioctl(TIOCGWINSZ) to query rows and columns in tty */
            ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
            // printf ("lines %d\n", w.ws_row);
            // printf ("columns %d\n", w.ws_col);

            // /* show off how perfectly we can measure it */
            int col;
            // for(row = 0; row < (w.ws_row - 4); row++)
            // {
            //     for(col = 0; col < w.ws_col; col++)
            //     {
            //         putchar('=');
            //     }
            //     putchar('\n');
            // }

            /* wrapped columns: 
             *      - problem statement (optional) 
             *      - natural language solution (optional) 
             * */
            unsigned short remaining_width = w.ws_col - tw.total;
            if(problem_statement && natural_language)
            {
                // case with both
                /* " statements | language |" is the min width */
                const unsigned short min_width = 23U;
                if((tw.total + min_width) > w.ws_col) // tty too narrow
                {
                    single_line_print = true;
                }
                else
                {
                    if((remaining_width % 2) == 0)
                    { // even case
                        // remove 3 preemptively for spaces and column
                        tw.problem_statement = (remaining_width / 2) - 3U;
                        tw.natural_language = (remaining_width / 2) - 3U;
                        tw.total = w.ws_col;
                    }
                    else 
                    { // odd case, the remainder goes to the problem statement
                        //remove 3 preemptively for spaces and column
                        tw.problem_statement = (remaining_width / 2) - 2U;
                        tw.natural_language = (remaining_width / 2) - 3U;
                        tw.total = w.ws_col;
                    }
                }
            }
            else if(problem_statement)
            {
                // problem statement column only
                /* " statement |" is the min width */
                const unsigned short min_width = 12U;
                if((tw.total + min_width) > w.ws_col) // tty too narrow
                {
                    single_line_print = true;
                }
                else
                {
                    // remove 3 preemptively for spaces and column
                    tw.problem_statement = remaining_width - 3U;
                    tw.total = w.ws_col;
                }   
            }
            else if(natural_language)
            {
                // only natural language column
                /* " language |" is min width */
                const unsigned short min_width = 11U;
                if((tw.total + min_width) > w.ws_col) // tty too narrow
                {
                    single_line_print = true;
                }
                else
                {
                    // remove 3 preemptively for spaces and column
                    tw.natural_language = remaining_width - 3U;
                    tw.total = w.ws_col;
                }
            }
            else
            {
                // no wrapped columns
                if((tw.total) > w.ws_col) // tty too narrow
                    single_line_print = true;
            }            

            if(single_line_print)
            {
                // case where we don't care about wrapping and just print as if
                // STDOUT is not a tty just like below
                report_results_tabulated_single_line(tw);
            }
            else
            {
                // case where we want to build a nice looking table
                // w.ws_col is a valid width we want to fill
                

                printf("| %-*s ", tw.problem_num, "num");
                if(report_time)
                {
                    printf("| %-*s ", 
                            tw.exe_time_width + tw.exe_time_precision + 4U, "exe time");
                }
                if(numeric)
                {
                    printf("| %-*s ", tw.numerical_solution, "answer");
                }
                if(natural_language)
                {
                    printf("| %-*s ", tw.natural_language, "language");
                }
                if(problem_statement)
                {
                    printf("| %-*s ", tw.problem_statement, "statement");
                }
                printf("|\n");
                print_width(tw.total, '=');

                for(i = 0; i < HIGHEST_PROBLEM_COMPLETED; i++)
                {
                    if(problems[i])
                    {
                        // start with the fixed-width stuff we know fits
                        printf("| %0*u ", tw.problem_num, solution_arr[i].problem_number);
                        if(report_time)
                            printf("| %*.*f ms ", 
                                    tw.exe_time_width, 
                                    tw.exe_time_precision,
                                    solution_arr[i].execution_time_ms);
                        if(numeric)
                            printf("| %*s ", 
                                    tw.numerical_solution, 
                                    solution_arr[i].numerical_solution);

                        // three cases for printing the potentially wrapped
                        // columns: 1) both, 2) natural lang 3) prob statement
                        if(problem_statement && natural_language)
                        {
                            // need to wrap both to potentially different
                            // lengths
                            _Bool end_prob = false;
                            _Bool end_lang = false;
                            int i_prob, i_lang; 
                            unsigned short cur_i;
                            i_prob = 0;
                            i_lang = 0;
                            while(!(end_lang && end_prob))
                            {
                                // once we run one of these out we want to just
                                // print the right width
                                if(end_lang)
                                {
                                    // fill natural language with spaces
                                    printf("| %*s ", tw.natural_language, "");
                                }
                                else
                                {
                                    // wrap natural language
                                    printf("| ");
                                    for(cur_i = 0U; cur_i < tw.natural_language; cur_i++)
                                    {
                                        if(solution_arr[i].natural_language_solution[i_lang] != '\0')
                                        {
                                            putchar(solution_arr[i].natural_language_solution[i_lang++]);
                                        }
                                        else
                                        {
                                            end_lang = true;
                                            putchar(' ');
                                        }
                                    }
                                    putchar(' ');
                                }

                                if(end_prob)
                                {
                                    printf("| %*s ", tw.problem_statement, "");
                                }
                                else
                                {
                                    // wrap problem statement
                                    // wrap natural language
                                    printf("| ");
                                    for(cur_i = 0U; cur_i < tw.problem_statement; cur_i++)
                                    {
                                        if(solution_arr[i].problem_statement[i_prob] != '\0')
                                        {
                                            putchar(solution_arr[i].problem_statement[i_prob++]);
                                        }
                                        else
                                        {
                                            end_prob = true;
                                            putchar(' ');
                                        }
                                    }
                                    putchar(' ');

                                }
                                
                                // do we need to keep wrapping?
                                if(end_prob && end_lang)
                                {
                                    // no this is the last pass
                                }
                                else
                                {
                                    // yes we have at least one more pass
                                    printf("|\n");
                                    printf("| %*s ", tw.problem_num, "");
                                    if(report_time)
                                        printf("| %*s ", tw.exe_time_precision + tw.exe_time_width + 4U, "");
                                    if(numeric)
                                        printf("| %*s ", tw.numerical_solution, "");
                                }
                            }
                        }
                        else if(problem_statement)
                        {
                            _Bool end_prob = false;
                            int i_prob; 
                            unsigned short cur_i;
                            i_prob = 0;
                            while(!(end_prob))
                            {
                                // wrap problem statement
                                printf("| ");
                                for(cur_i = 0U; cur_i < tw.problem_statement; cur_i++)
                                {
                                    if(solution_arr[i].problem_statement[i_prob] != '\0')
                                    {
                                        putchar(solution_arr[i].problem_statement[i_prob++]);
                                    }
                                    else
                                    {
                                        end_prob = true;
                                        putchar(' ');
                                    }
                                }
                                putchar(' ');

                                // do we need to keep wrapping?
                                if(end_prob)
                                {
                                    // no this is the last pass
                                }
                                else
                                {
                                    // yes we have at least one more pass
                                    printf("|\n");
                                    printf("| %*s ", tw.problem_num, "");
                                    if(report_time)
                                        printf("| %*s ", tw.exe_time_precision + tw.exe_time_width + 4U, "");
                                    if(numeric)
                                        printf("| %*s ", tw.numerical_solution, "");
                                }
                            }
                        }
                        else if(natural_language)
                        {
                            _Bool end_lang = false;
                            int i_lang; 
                            unsigned short cur_i;
                            i_lang = 0;

                            while(!(end_lang))
                            {
                                // wrap natural language
                                printf("| ");
                                for(cur_i = 0U; cur_i < tw.natural_language; cur_i++)
                                {
                                    if(solution_arr[i].natural_language_solution[i_lang] != '\0')
                                    {
                                        putchar(solution_arr[i].natural_language_solution[i_lang++]);
                                    }
                                    else
                                    {
                                        end_lang = true;
                                        putchar(' ');
                                    }
                                }
                                putchar(' ');

                                // do we need to keep wrapping?
                                if(end_lang)
                                {
                                    // no this is the last pass
                                }
                                else
                                {
                                    // yes we have at least one more pass
                                    printf("|\n");
                                    printf("| %*s ", tw.problem_num, "");
                                    if(report_time)
                                        printf("| %*s ", tw.exe_time_precision + tw.exe_time_width + 4U, "");
                                    if(numeric)
                                        printf("| %*s ", tw.numerical_solution, "");
                                }
                            }
                            
                        }

                        // problem printing completed
                        printf("|\n");
                        print_width(tw.total, '-');
                    }
                }
            }
        }
        else // STDOUT is not a tty!
        {
            /* this false case happens more than you'd think. Some common
             * examples:
             *      - using "| grep", or other pipes
             *      - routing to a file for storage, e.g. " > test.txt"
             * */

            /* we're going to assume an infinite max width, so the header width
             * for each field needs to be set by the largest */
           report_results_tabulated_single_line(tw); 
        }
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        //define something for Windows (32-bit and 64-bit, this part is common)
        printf("Windows tabulation not implemented yet, use non-tabulated "
                "output\n");
#elif __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
        // Other kinds of Apple platforms
        printf("Mac tabulation not implemented yet, use non-tabulated "
                "output\n");
    #else
    #   error "Unsupported Apple platform"
    #endif
#else
#   error "Unsupported compiler"
#endif
    }
    else /* non-tabulated output */
    {
        for(i = 0; i < HIGHEST_PROBLEM_COMPLETED; i++)
        {
            if(problems[i])
            {
                printf("Problem %03u:\n", solution_arr[i].problem_number);
                if(problem_statement)
                    printf("Problem Statement: %s\n", 
                            solution_arr[i].problem_statement);
                if(numeric)
                    printf("Numeric Solution: %s\n",
                            solution_arr[i].numerical_solution);
                if(natural_language)
                    printf("Natural Language Solution: %s\n",
                            solution_arr[i].natural_language_solution);
                if(report_time)
                    printf("Execution Time: %.3f milliseconds\n", 
                            solution_arr[i].execution_time_ms);
            }
        }
    }

    return EXIT_SUCCESS;
}

int main(int argc, char * argv[])
{
    int i, ret; // max < 1,000: int is sufficiently large

    /* Run the arg parser to handle user input / program options */
    argp_parse(&parser, argc, argv, 0, 0, 0);

    /* run each problem indicated by program options */
    for(i = 0; i < HIGHEST_PROBLEM_COMPLETED; i++)
    {
        if(problems[i])
        {
            ret = problem_func_ptrs[i](&solution_arr[i]); 
            if(ret == EXIT_FAILURE)
            {
                printf("ERROR: problem %d returned EXIT_FAILURE\n", i);
                return EXIT_FAILURE;
            }
        }        
    } 

    /* report results according to program options */
    ret = report_results();
    if(ret == EXIT_FAILURE)
    {
        printf("ERROR: report_results() returned EXIT_FAILURE\n");
        return EXIT_FAILURE;
    }

    /* free the strings allocated to the heap by each problem run */
    for(i = 0; i < HIGHEST_PROBLEM_COMPLETED; i++)
    {
        if(problems[i])
        {
            free_problem_solution_strings(&solution_arr[i]);
        }
    }

    return EXIT_SUCCESS;
}

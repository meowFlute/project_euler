#include <stdio.h>  	// printf
#include <stdlib.h>	// standard library functions
#include <stdbool.h>	// true, false
#include <errno.h>      // errno
#include <string.h>	// string functions
#include <math.h>       // log
#ifdef __linux__
#include <sys/ioctl.h>  // ioctl
#include <unistd.h>     // STDOUT_FILENO
#endif

#include "project_euler.h"  // incudes display_results.h as well as important
			    // global variables

#define TIME_ADDER 6U

typedef struct tabulated_widths{
    unsigned short total;
    unsigned short problem_num;
    unsigned short exe_time_width;
    unsigned short exe_time_precision;
    unsigned short numerical_solution;
    unsigned short problem_statement;
    unsigned short natural_language;
} tabulated_widths;

static int print_width(unsigned short width, char c)
{
    unsigned short i;
    int ret;
    for(i = 0; i < width; i++)
    {
        ret = putchar(c);
        if( ret == EOF )
        {
            printf("ERROR: function print_width: putchar(%c) returned EOF\n", c);
            return EXIT_FAILURE;
        }
    }
    ret = putchar('\n');
    if( ret == EOF )
    {
        printf("ERROR: function print_width: putchar('\\n') returned EOF\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int single_line(tabulated_widths tw)
{
    /* There are two cases where we'd want to do this:
     *  1) not a tty
     *  2) not a wide enough tty
     * In those cases, we'll simply assume infinite width. We also assume all
     * strings have no newline characters.
     * */
    int i, ret;

    // since we're reporting all of these on one line, we need to set the
    // length of the strings that would normally be wrapped.
    tw.natural_language = (unsigned short)strlen("natural language solution");
    tw.problem_statement = (unsigned short)strlen("problem statement");
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

    tw.total = 0;
    printf("| %-*s ", (int)(tw.problem_num), "num");
    tw.total += tw.problem_num + 3U;
    if(report_cpu_time)
    {
        printf("| %-*s ", 
                (int)(tw.exe_time_width + TIME_ADDER-3), 
                "cpu time");
        tw.total += tw.exe_time_width;
    }
    if(report_real_time)
    {
        printf("| %-*s ", 
                (int)(tw.exe_time_width + TIME_ADDER-3), 
                "cpu time");
        tw.total += tw.exe_time_width;
    }
    if(numeric)
    {
        printf("| %-*s ", (int)(tw.numerical_solution), "answer");
        tw.total += tw.numerical_solution + 3U;
    }
    if(natural_language)
    {
        printf("| %-*s ", (int)(tw.natural_language), "natural language solution");
        tw.total += tw.natural_language + 3U;
    }
    if(problem_statement)
    {
        printf("| %-*s ", (int)(tw.problem_statement), "problem statement");
        tw.total += tw.problem_statement + 3U;
    }
    printf("|\n");
    tw.total += 1U;

    ret = print_width(tw.total, '=');
    if(ret == EXIT_FAILURE)
    {
        printf("ERROR: function single_line: print_width(tw.total, '=')" 
                " returns EXIT_FAILURE\n");
        return EXIT_FAILURE;
    }

    for(i = 0; i < HIGHEST_PROBLEM_COMPLETED; i++)
    {
        if(problems[i])
        {
            printf("| %0*u ", 
                    (int)(tw.problem_num), 
                    solution_arr[i].problem_number);
            if(report_cpu_time)
                printf("| %*.*f ms ", 
                        (int)(tw.exe_time_width), 
                        (int)(tw.exe_time_precision),
                        solution_arr[i].cpu_time_ms);
            if(report_real_time)
                printf("| %*.*f ms ", 
                        (int)(tw.exe_time_width), 
                        (int)(tw.exe_time_precision),
                        solution_arr[i].real_time_ms);
            if(numeric)
                printf("| %*s ", 
                        (int)(tw.numerical_solution), 
                        solution_arr[i].numerical_solution);
            if(natural_language)
                printf("| %-*s ", 
                        (int)(tw.natural_language), 
                        solution_arr[i].natural_language_solution);
            if(problem_statement)
                printf("| %-*s ", 
                        (int)(tw.problem_statement), 
                        solution_arr[i].problem_statement);
            printf("|\n");
            ret = print_width(tw.total, '-');
            if(ret == EXIT_FAILURE)
            {
                printf("ERROR: function single_line: print_width(tw.total, '-')" 
                        " returns EXIT_FAILURE\n");
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;
}

int display_results(void)
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
        tw.total = 0;

        /* fixed width columns:
         *      - problem number
         *      - time elapsed (optional) 
         *      - numeric_only solution (optional, default)
         * */ 
        tw.problem_num = (unsigned short)3U;
        tw.exe_time_precision = (unsigned short)6U;
        tw.exe_time_width = (unsigned short)1U;
        tw.numerical_solution = (unsigned short)6U;
        tw.problem_statement = 0;
        tw.natural_language = 0;
        for(i = 0; i < HIGHEST_PROBLEM_COMPLETED; i++)
        {
            if(problems[i])
            {
                if(report_cpu_time)
                {
                    if(solution_arr[i].cpu_time_ms > 1.0)
                    {
                        tmp_time_width = (unsigned short)floor(
                                log10(solution_arr[i].cpu_time_ms)) + 2 + tw.exe_time_precision;
                        if(tmp_time_width > tw.exe_time_width)
                            tw.exe_time_width = tmp_time_width;
                    }
                }

                if(report_real_time)
                {
                    if(solution_arr[i].cpu_time_ms > 1.0)
                    {
                        tmp_time_width = (unsigned short)floor(
                                log10(solution_arr[i].real_time_ms)) + 2 + tw.exe_time_precision;
                        if(tmp_time_width > tw.exe_time_width)
                            tw.exe_time_width = tmp_time_width;
                    }
                }
                
                if((!report_real_time) && (!report_cpu_time))
                {
                    tw.exe_time_width = 0;
                    tw.exe_time_precision = 0;
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
                    tw.numerical_solution = 0;
                }
            }
        }

        /* calculate how much width we have left for the wrapped columns */
        tw.total += tw.problem_num + 3U; // adding 3 for spaces and column
        if(report_real_time)
            tw.total += tw.exe_time_precision + tw.exe_time_width;
        if(report_cpu_time)
            tw.total += tw.exe_time_precision + tw.exe_time_width;
        if(numeric)
            tw.total += tw.numerical_solution + 3U; // spaces, '|' 
        tw.total += 1U; // for the final column

#if __linux__

        /* w.ws_col contains the column width we need to know to keep
         * formatting nice after we use ioctl to check the term */
        int ret = isatty(STDOUT_FILENO);
        if(ret == 1)
        {
            /* STDOUT is a tty */
            struct winsize w;
            w.ws_col = (unsigned short)0;
            w.ws_row = (unsigned short)0;
            w.ws_xpixel = (unsigned short)0;
            w.ws_ypixel = (unsigned short)0;

            /* use ioctl(TIOCGWINSZ) to query rows and columns in tty */
            ret = ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
            if(ret == -1)
            {
                printf("ERROR: ioctl returned an error. errno=%d message: %s\n",
                        errno,
                        strerror(errno));
            }

            /* wrapped columns: 
             *      - problem statement (optional) 
             *      - natural language solution (optional) 
             * */
            unsigned short remaining_width = w.ws_col - tw.total;
            if(problem_statement && natural_language)
            {
                // case with both
                /* " statements | language |" is the min width */
                const unsigned short min_width = (unsigned short)23U;
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
                const unsigned short min_width = (unsigned short)12U;
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
                const unsigned short min_width = (unsigned short)11U;
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
                ret = single_line(tw);
                if(ret == EXIT_FAILURE)
                {
                    printf("ERROR: single_line(tw) returned EXIT_FAILURE\n");
                    return EXIT_FAILURE;
                }
            }
            else
            {
                // case where we want to build a nice looking table
                // w.ws_col is a valid width we want to fill
                

                printf("| %-*s ", (int)(tw.problem_num), "num");
                if(report_cpu_time)
                {
                    printf("| %-*s ", 
                            (int)(tw.exe_time_width + TIME_ADDER - 3), 
                            "cpu time");
                }
                if(report_real_time)
                {
                    printf("| %-*s ", 
                            (int)(tw.exe_time_width + TIME_ADDER - 3), 
                            "real time");
                }
                if(numeric)
                {
                    printf("| %-*s ", (int)(tw.numerical_solution), "answer");
                }
                if(natural_language)
                {
                    printf("| %-*s ", (int)(tw.natural_language), "language");
                }
                if(problem_statement)
                {
                    printf("| %-*s ", (int)(tw.problem_statement), "statement");
                }
                printf("|\n");
                ret = print_width(tw.total, '=');
                if(ret == EXIT_FAILURE)
                {
                    printf("ERROR: print_width(tw.total, '=') returned EXIT_FAILURE\n");
                    return EXIT_FAILURE;
                }

                for(i = 0; i < HIGHEST_PROBLEM_COMPLETED; i++)
                {
                    if(problems[i])
                    {
                        // start with the fixed-width stuff we know fits
                        printf("| %0*u ", 
                                (int)(tw.problem_num), 
                                solution_arr[i].problem_number);
                        if(report_cpu_time)
                            printf("| %*.*f ms ", 
                                    (int)(tw.exe_time_width), 
                                    (int)(tw.exe_time_precision),
                                    solution_arr[i].cpu_time_ms);
                        if(report_real_time)
                            printf("| %*.*f ms ", 
                                    (int)(tw.exe_time_width), 
                                    (int)(tw.exe_time_precision),
                                    solution_arr[i].real_time_ms);
                        if(numeric)
                            printf("| %*s ", 
                                    (int)(tw.numerical_solution), 
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
                                    printf("| %*s ", 
                                            (int)(tw.natural_language), "");
                                }
                                else
                                {
                                    // wrap natural language
                                    printf("| ");
                                    for(cur_i = 0; cur_i < tw.natural_language; cur_i++)
                                    {
                                        if(solution_arr[i].natural_language_solution[i_lang] != '\0')
                                        {
                                            ret = putchar(solution_arr[i].natural_language_solution[i_lang++]);
                                            if( ret == EOF )
                                            {
                                                printf("ERROR: function print_width: putchar(%c) returned EOF\n", 
                                                        solution_arr[i].natural_language_solution[i_lang-1]);
                                                return EXIT_FAILURE;
                                            }
                                        }
                                        else
                                        {
                                            end_lang = true;
                                            ret = putchar(' ');
                                            if( ret == EOF )
                                            {
                                                printf("ERROR: function print_width: putchar(' ') returned EOF\n");
                                                return EXIT_FAILURE;
                                            }
                                        }
                                    }
                                    ret = putchar(' ');
                                    if( ret == EOF )
                                    {
                                        printf("ERROR: function print_width: putchar(' ') returned EOF\n");
                                        return EXIT_FAILURE;
                                    }
                                }

                                if(end_prob)
                                {
                                    printf("| %*s ", (int)(tw.problem_statement), "");
                                }
                                else
                                {
                                    // wrap problem statement
                                    // wrap natural language
                                    printf("| ");
                                    for(cur_i = 0; cur_i < tw.problem_statement; cur_i++)
                                    {
                                        if(solution_arr[i].problem_statement[i_prob] != '\0')
                                        {
                                            ret = putchar(solution_arr[i].problem_statement[i_prob++]);
                                            if( ret == EOF )
                                            {
                                                printf("ERROR: function print_width: putchar(%c) returned EOF\n", 
                                                        solution_arr[i].natural_language_solution[i_prob-1]);
                                                return EXIT_FAILURE;
                                            }
                                        }
                                        else
                                        {
                                            end_prob = true;
                                            ret = putchar(' ');
                                            if( ret == EOF )
                                            {
                                                printf("ERROR: function print_width: putchar(' ') returned EOF\n");
                                                return EXIT_FAILURE;
                                            }
                                        }
                                    }
                                    ret = putchar(' ');
                                    if( ret == EOF )
                                    {
                                        printf("ERROR: function print_width: putchar(' ') returned EOF\n");
                                        return EXIT_FAILURE;
                                    }
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
                                    printf("| %*s ", (int)(tw.problem_num), "");
                                    if(report_cpu_time)
                                        printf("| %*s ", 
                                                (int)(tw.exe_time_width + TIME_ADDER - 3), 
                                                "");
                                    if(report_real_time)
                                        printf("| %*s ", 
                                                (int)(tw.exe_time_width + TIME_ADDER - 3), 
                                                "");
                                    if(numeric)
                                        printf("| %*s ", 
                                                (int)(tw.numerical_solution), 
                                                "");
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
                                for(cur_i = 0; cur_i < tw.problem_statement; cur_i++)
                                {
                                    if(solution_arr[i].problem_statement[i_prob] != '\0')
                                    {
                                        ret = putchar(solution_arr[i].problem_statement[i_prob++]);
                                        if( ret == EOF )
                                        {
                                            printf("ERROR: function print_width: putchar(%c) returned EOF\n", 
                                                    solution_arr[i].natural_language_solution[i_prob-1]);
                                            return EXIT_FAILURE;
                                        }
                                    }
                                    else
                                    {
                                        end_prob = true;
                                        ret = putchar(' ');
                                        if( ret == EOF )
                                        {
                                            printf("ERROR: function print_width: putchar(' ') returned EOF\n");
                                            return EXIT_FAILURE;
                                        }
                                    }
                                }
                                ret = putchar(' ');
                                if( ret == EOF )
                                {
                                    printf("ERROR: function print_width: putchar(' ') returned EOF\n");
                                    return EXIT_FAILURE;
                                }

                                // do we need to keep wrapping?
                                if(end_prob)
                                {
                                    // no this is the last pass
                                }
                                else
                                {
                                    // yes we have at least one more pass
                                    printf("|\n");
                                    printf("| %*s ", (int)(tw.problem_num), "");
                                    if(report_cpu_time)
                                        printf("| %*s ", 
                                                (int)(tw.exe_time_width + TIME_ADDER - 3), 
                                                "");
                                    if(report_real_time)
                                        printf("| %*s ", 
                                                (int)(tw.exe_time_width + TIME_ADDER - 3), 
                                                "");
                                    if(numeric)
                                        printf("| %*s ", 
                                                (int)(tw.numerical_solution), 
                                                "");
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
                                for(cur_i = 0; cur_i < tw.natural_language; cur_i++)
                                {
                                    if(solution_arr[i].natural_language_solution[i_lang] != '\0')
                                    {
                                        ret = putchar(solution_arr[i].natural_language_solution[i_lang++]);
                                        if( ret == EOF )
                                        {
                                            printf("ERROR: function print_width: putchar(%c) returned EOF\n", 
                                                    solution_arr[i].natural_language_solution[i_lang-1]);
                                            return EXIT_FAILURE;
                                        }
                                    }
                                    else
                                    {
                                        end_lang = true;
                                        ret = putchar(' ');
                                        if( ret == EOF )
                                        {
                                            printf("ERROR: function print_width: putchar(' ') returned EOF\n");
                                            return EXIT_FAILURE;
                                        }
                                    }
                                }
                                ret = putchar(' ');
                                if( ret == EOF )
                                {
                                    printf("ERROR: function print_width: putchar(' ') returned EOF\n");
                                    return EXIT_FAILURE;
                                }
                                // do we need to keep wrapping?
                                if(end_lang)
                                {
                                    // no this is the last pass
                                }
                                else
                                {
                                    // yes we have at least one more pass
                                    printf("|\n");
                                    printf("| %*s ", (int)(tw.problem_num), "");
                                    if(report_cpu_time)
                                        printf("| %*s ", 
                                                (int)(tw.exe_time_width + TIME_ADDER - 3), 
                                                "");
                                    if(report_real_time)
                                        printf("| %*s ", 
                                                (int)(tw.exe_time_width + TIME_ADDER - 3), 
                                                "");
                                    if(numeric)
                                        printf("| %*s ", 
                                                (int)(tw.numerical_solution), 
                                                "");
                                }
                            }
                            
                        }

                        // problem printing completed
                        printf("|\n");
                        ret = print_width(tw.total, '-');
                        if(ret == EXIT_FAILURE)
                        {
                            printf("ERROR: print_width returned EXIT_FAILURE\n");
                            return EXIT_FAILURE;
                        }   
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
             * the man page for isatty says that one of two (or three) errno
             * values should be set and we can check them:
             *      1) EBADF if STDOUT is not a valid file descriptor
             *      2) ENOTTY (sometimes EINVAL) if it just isn't a tty
             * */
            if(errno == EBADF)
            {
                printf("ERROR: STDOUT not a valid file descriptor\n");
                return EXIT_FAILURE;
            }
            else if((errno == ENOTTY) || (errno == EINVAL))
            {
                /* Based on the man pages for isatty we're ok with these values
                 * of errno, and they should happen for pipes and such */
                /* we're going to assume an infinite max width, so the header 
                 * width for each field needs to be set by the largest */
                ret = single_line(tw); 
                if(ret == EXIT_FAILURE)
                {
                    printf("ERROR: single_line returned EXIT_FAILURE\n");
                    return EXIT_FAILURE;
                }
            }
            else
            {
                printf("ERROR: unexpexted errno value: errno=%d message: %s\n",
                        errno,
                        strerror(errno));
                return EXIT_FAILURE;
            }
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
                if(report_cpu_time)
                    printf("CPU Time: %.3f milliseconds\n", 
                            solution_arr[i].cpu_time_ms);
                if(report_real_time)
                    printf("Calendar Time: %.3f milliseconds\n", 
                            solution_arr[i].real_time_ms);
            }
        }
    }

    return EXIT_SUCCESS;
}


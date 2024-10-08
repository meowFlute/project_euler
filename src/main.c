/* LIBRARY HEADER FILES */
#include <argp.h>       // argp functions
#include <stdbool.h>    // false
#include <stdlib.h>     // EXIT_SUCCESS
#include <string.h>     // strtok
#include <errno.h>      // errno
#include <unistd.h>     // sysconf
#include <inttypes.h>   // uint64_t and PRIu64

/* LOCAL HEADER FILES */
#include "project_euler.h"

// macros to make DEF2STR(HIGHEST_PROBLEM_COMPLETED) = "8" (or whatever number)
// used below to make it so argp_program_version 
#define DEF2STRX(x) #x
#define DEF2STR(x) DEF2STRX(x)
#define BUFSIZE 256

/* array of problem run flags */
_Bool problems[HIGHEST_PROBLEM_COMPLETED];
_Bool argument_encountered = false;
_Bool report_real_time = false;
_Bool report_cpu_time = false;
_Bool numeric = true; //true by default, option sets false
_Bool natural_language = false;
_Bool problem_statement = false;
_Bool tabulated = false;
_Bool system_info = true;

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
    problem_011,
    problem_012,
    problem_013,
    problem_014,
    problem_015,
    problem_016,
    problem_017,
    problem_018,
    problem_019,
    problem_020,
    problem_021,
    problem_022
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
        {NULL, 0, NULL, 0,
            "General Formatting Option Flags:", 0},
        {"no-system-info", (int)('i'), NULL, 0,
            "disables system info output", 0},
        {"tabulated", (int)('z'), NULL, 0,
            "report solutions in a table", 0},
        {NULL, 0, NULL, 0,
            "Problem Selection Options:", 2},
        {"problems", (int)('p'), "CSV_PROBS", 0,
            "comma-separated (no spaces) integers representing problems to run."
            " e.g. 1,5,23", 2},
        {"range", (int)('r'), "RANGE_PROBS", 0,
            "START and END (inclusive) problem numbers separated by hyphen with no spaces,"
            " e.g. 5-10", 2},
        {"last-five", (int)('l'), NULL, 0,
            "same as --range=(HIGHEST_PROBLEM_COMPLETED-5)-(HIGHEST_PROBLEM_COMPLETED)", 2},
        {"all", (int)('a'), NULL, 0,
            "run all of the problems", 2},
        {NULL, 0, NULL, 0,
            "Reported Field Option Flags:", 3},
        {"real-time", (int)('t'), NULL, 0,
            "track and report calendar time of each problem solution", 3},
        {"cpu-time", (int)('c'), NULL, 0,
            "track and report cpu time of each problem solution", 3},
        {"no-numeric-only", (int)('n'), NULL, 0,
            "report only numbers for solutions", 3},
        {"problem-statement", (int)('s'), NULL, 0,
            "include problem statements in the results", 3},
        {"natural-language", (int)('x'), NULL, 0,
            "report full-length text solutions in natural language", 3},
        {NULL, 0, NULL, 0,
            "Help and Information:", 4},
        { NULL, 0, NULL, 0, NULL, 0 }
    };

static struct argp parser = { options, project_euler_parser, NULL, doc, 
                                NULL, NULL, NULL };

static error_t project_euler_parser(int key, char * arg, 
        struct argp_state * state)
{
    if(key == ARGP_KEY_END)
    {
        if(!argument_encountered)
        {
            argp_error(state, "at least one Problem Selection Option is required "
                    "possible Problem Selection Options: -alpr ... (-t,--time and others don't count)");
        }
    }
    if(key == ((int)('n')))
        numeric = false;
    if(key == ((int)('s')))
        problem_statement = true;
    if(key == ((int)('x')))
        natural_language = true;
    if(key == ((int)('i')))
        system_info = false;
    if(key == ((int)('z')))
        tabulated = true;
    if(key == ((int)('t')))
        report_real_time = true;
    if(key == ((int)('c')))
        report_cpu_time = true;
    if(key == ((int)('a')))
    {
        argument_encountered = true;
        int i;
        for(i = 0; i < HIGHEST_PROBLEM_COMPLETED; i++)
        {
            problems[i] = true;
        }
    }
    if(key == ((int)('l')))
    {
        argument_encountered = true;
        int i;
        for(i = HIGHEST_PROBLEM_COMPLETED-6; i < HIGHEST_PROBLEM_COMPLETED; i++)
        {
            problems[i] = true;
        }
    }
    if(key == ((int)('r')))
    {
        argument_encountered = true;
        /* strdup uses malloc, arg_malloc must be freed later */
        char * arg_malloc = strdup(arg);
        if(arg_malloc == NULL)
        {
            fprintf(stderr, "arg_copy malloc failed, errno=%d: %s\n", errno, strerror(errno));
            return EXIT_FAILURE;
        }
        /* strsep will set the pointer to null without freeing, so we pass it a
         * copy and retain the original to free later */
        char * arg_copy = arg_malloc; 
        char * ptr1 = strsep(&arg_copy, "-"); // start of range
        char * ptr2 = strsep(&arg_copy, "-"); // end of range
        char * ptr3 = strsep(&arg_copy, "-"); // this one should be NULL
        if((ptr1 == NULL) || (ptr2 == NULL) || (ptr3 != NULL))
            argp_error(state, "incorrect arguments for --range");
        int i = 0;
        int start = atoi(ptr1);
        int end = atoi(ptr2);
        if((start == 0) || (end == 0))
            argp_error(state, "incorrect arguments for --range");
        for(i = start-1; i < end; i++)
        {
            problems[i] = true;
        }

        free(arg_malloc);
    }
    if(key == ((int)('p')))
    {
        argument_encountered = true;
        /* strdup uses malloc, arg_malloc must be freed later */
        char * arg_malloc = strdup(arg);
        if(arg_malloc == NULL)
        {
            fprintf(stderr, "arg_copy malloc failed, errno=%d: %s\n", errno, strerror(errno));
            return EXIT_FAILURE;
        }
        /* strsep will set the pointer to null without freeing, so we pass it a
         * copy and retain the original to free later */
        char * arg_copy = arg_malloc; 
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

        free(arg_malloc);
    }

    return 0;
}


int main(int argc, char * argv[])
{
    int i, ret, pagesize; // max < 1,000: int is sufficiently large
    double total_memory, available_memory;
    struct timespec clock_res;

    /* init all problems to false */
    for(i = 0; i < HIGHEST_PROBLEM_COMPLETED; i++)
    {
        problems[i] = false;
    }

    /* Run the arg parser to handle user input / program options */
    error_t argp_ret = argp_parse(&parser, argc, argv, 0, NULL, NULL);
    if(argp_ret != 0)
    {
        fprintf(stderr, "error: argp_parse returned %s\n", strerror(argp_ret));
        return argp_ret;
    }

#ifdef __linux__
    if(system_info)
    {
        /* read processor info from lspcu */
        char* cmd = "lscpu";
        char* buffer = NULL;
        char* bogomips = NULL;
        char* bogomips_malloc = NULL;
        char* cpu_info = NULL;
        char* cpu_info_malloc = NULL;
        char* max_mhz = NULL;
        char* max_mhz_malloc = NULL;
        const char delim = ':';
        size_t line_len;
        FILE* fp;

        if ((fp = popen(cmd, "r")) == NULL) 
        {
            fprintf(stderr, "error opening 'lscpu' pipe, skipping\n");
        }
        else
        {

            while (getline(&buffer, &line_len, fp) != -1) 
            {
                if(strstr(buffer, "Model name") != NULL)
                {
                    cpu_info_malloc = strdup(buffer);
                    cpu_info = cpu_info_malloc;
                    strsep(&cpu_info, &delim); // ignoring return on purpose
                    while(cpu_info[0] == ' ')
                        cpu_info++; // trim leading whitespace
                    i = 0;
                    while(cpu_info[i] != '\0')
                    {
                        if(cpu_info[i] == '\n')
                        {
                            // null-terminate at the newline
                            cpu_info[i] = '\0';
                        }
                        i++;
                    }
                }
                if(strstr(buffer, "CPU max MHz") != NULL)
                {
                    max_mhz_malloc = strdup(buffer);
                    max_mhz = max_mhz_malloc;
                    strsep(&max_mhz, &delim); // ignoring return on purpose
                    while(max_mhz[0] == ' ')
                        max_mhz++; // trim leading whitespace
                    i = 0;
                    while(max_mhz[i] != '\0')
                    {
                        if(max_mhz[i] == '\n')
                        {
                            // null-terminate at the newline
                            max_mhz[i] = '\0';
                        }
                        i++;
                    }
                }
                if(strstr(buffer, "BogoMIPS") != NULL)
                {
                    bogomips_malloc = strdup(buffer);
                    bogomips = bogomips_malloc;
                    strsep(&bogomips, &delim); // ignoring return on purpose
                    while(bogomips[0] == ' ')
                        bogomips++; // trim leading whitespace
                    i = 0;
                    while(bogomips[i] != '\0')
                    {
                        if(bogomips[i] == '\n')
                        {
                            // null-terminate at the newline
                            bogomips[i] = '\0';
                        }
                        i++;
                    }
                }
            }

            if (pclose(fp)) 
            {
                printf("Command not found or exited with error status\n");
                return -1;
            }
        }

        /* print some standard system information */
        const int str_width = 35;
        if((ret = clock_getres(CLOCK_MONOTONIC, &clock_res)) != 0)
            perror("main.c: clock_getres():");
        pagesize = sysconf(_SC_PAGESIZE);
        total_memory = (double)pagesize * (double)sysconf(_SC_PHYS_PAGES) / (1024.0 * 1024.0); 
        available_memory = (double)pagesize * (double)sysconf(_SC_AVPHYS_PAGES) / (1024.0 * 1024.0); 

        printf("%*s: VERSION %d.%d.%d\n", str_width, "SCOTT'S PROJECT EULER SOFTWARE", MAIN_VERSION, HIGHEST_PROBLEM_COMPLETED, SUB_VERSION);
        printf("%*s: %s\n", str_width, "processor model info", cpu_info);
        printf("%*s: %s MHz\n", str_width, "processor max clock freq", max_mhz);
        printf("%*s: %s MIPS\n", str_width, "BogoMIPS", bogomips);
        printf("%*s: %ld\n", str_width, "number of configured processors", sysconf(_SC_NPROCESSORS_CONF));
        printf("%*s: %ld\n", str_width, "number of available processors", sysconf(_SC_NPROCESSORS_ONLN));
        printf("%*s: %ld\n", str_width, "CLOCKS_PER_SEC", (long int)CLOCKS_PER_SEC);
        printf("%*s: %ld nanoseconds\n", str_width, "CLOCK_MONOTONIC resolution", clock_res.tv_nsec);
        printf("%*s: %7.1f MB\n", str_width, "total memory", total_memory); 
        printf("%*s: %7.1f MB\n", str_width, "available memory", available_memory);
        printf("%*s: %d bytes\n", str_width, "page size", pagesize);
        printf("\n");

        // getline mallocd buffer, strdup mallocd cpu_info and max_mhz
        free(buffer);
        if(cpu_info_malloc != NULL)
            free(cpu_info_malloc);
        if(max_mhz_malloc != NULL)
            free(max_mhz_malloc);
        if(bogomips_malloc != NULL)
            free(bogomips_malloc);
    }
#endif
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
    ret = display_results();
    if(ret == EXIT_FAILURE)
    {
        printf("ERROR: display_results() returned EXIT_FAILURE\n");
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

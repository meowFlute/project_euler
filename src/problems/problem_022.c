#include "problem_022.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

#define INITIAL_ARR_SIZE 5000
#define BUFF_SIZE 100
#define STR_MAX (BUFF_SIZE-1)

typedef struct name_score_t {
    int alpha_score;
    int total_score;
} name_score_t;

/* Input arguments:
 *  1) name_score_t* &ns_p: name_score_t pointer passed by reference. This will
 *  be malloc'd into a name_score_t array in function so the pointer will
 *  change, you might as well assign it to NULL prior to passing or just never
 *  initialize it, it will be overwritten. 
 *  2) int &num_names: pass an int by reference and it will be overwritten with
 *  the number of elements (and length of the array allocated)
 *  3) char* f_path: path to get to the names file "resources/0022_names.txt"
 *
 * Return Values:
 *  args 1 and 2 will be overwritten with output values
 *
 *  returning int RETURN_SUCCESS (0) is good
 *  returning anything else is an errno value and can be read with strerror
 *  */
int read_name_file_to_memory(name_score_t** ns_pp, int* num_names, const char* f_path)
{
    int ret;
    long f_size;
    int max_num_names;
    int name_i, str_i;
    char* f_str = NULL;
    char* tok = NULL;
    FILE* f_p = NULL;

    /* open file */
    if((f_p = fopen(f_path, "r")) == NULL)
    {
        perror("problem_022: fopen failed");
        return errno;
    }

    /* how long is this file? store that in f_size */
    if((ret = fseek(f_p, 0, SEEK_END)) != 0)
    {
        ret = errno;
        perror("problem_022: fseek(SEEK_END) failed");
        fclose(f_p);
        return ret;
    }
    if((f_size = ftell(f_p)) == -1)
    {
        ret = errno;
        perror("problem_022: ftell failed");
        fclose(f_p);
        return ret;
    }
    rewind(f_p); // no return value on rewind

    /* allocate the buffer size to be large enough to hold the whole file */
    f_str = (char*)malloc(f_size + 1); // +1 for the null-termination char
    
    /* read the entire block of memory into the buffer */
    if((ret = fread(f_str, (size_t)f_size, 1, f_p)) != 1)
    {
        ret = errno;
        perror("problem_022: fread failed");
        fclose(f_p);
        return ret;
    }

    /* close the file */
    if((fclose(f_p)) != 0)
    {
        perror("problem_022: fclose failed");
        return errno;
    }

    /* take the tokens out of the string and put them into an array */

    /* create array block, then realloc later as needed */
    *num_names = 0;
    max_num_names = INITIAL_ARR_SIZE;
    *ns_pp = (name_score_t*)malloc(sizeof(name_score_t)*max_num_names);
    if(*ns_pp == NULL)
    {
        perror("problem_022: error: malloc failure");
        return errno;
    }

    /* I'll use " as a delimiter and discard the "," tokens */
    tok = strtok(f_str, "\"");
    while(tok != NULL)
    {
        if((strncmp(tok, ",", 1)) == 0)
        {
            // discard the commas between names: "NAME1","NAME2
            tok = strtok(NULL, "\"");
            continue;
        }
        else
        {
            if(strlen(tok) > 0)
            {
                // we assume this should be the name, store in the next index
                name_i = (*num_names)++;
                
                /* calculate alpha_score */
                (*ns_pp)[name_i].alpha_score = 0;
                str_i = 0;
                while(tok[str_i] != '\0')
                {
                    // A is 1, a is 1 as well; Z is 26, z is 26, as well
                    (*ns_pp)[name_i].alpha_score += toupper(tok[str_i++]) - 'A' + 1;
                }

                /* calculate total score */
                (*ns_pp)[name_i].total_score = (*num_names)*((*ns_pp)[name_i].alpha_score);
            }
            else
            {
                fprintf(stderr, "problem_022: error: unexpected token of length 0\n");
            }
        }
        
        /* reallocate more as needed */
        if(max_num_names <= *num_names)
        { 
            max_num_names += 500;
            *ns_pp = (name_score_t*)realloc(*ns_pp, sizeof(name_score_t)*max_num_names);
            if(*ns_pp == NULL)
            {
                perror("problem_022: error: realloc failure");
                return errno;
            }
        }

        /* get the next token */
        tok = strtok(NULL, "\"");
    }

    /* free the original pointer */
    free(f_str);

    return RETURN_SUCCESS;
}

int problem_022(problem_solution *ps)
{
    clock_t start_cpu, end_cpu; 
    struct timespec start_real, end_real;
    double cpu_time_used_ms, real_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;

    ps->problem_number = 22U;
    ps->problem_statement = strdup("Using the file resources/0022_names.txt and"
            " a whole bunch of rules, total up the value of all the names in the file");

    /* start timing */
    start_cpu = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &start_real)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    //Solve problem 022
    name_score_t* ns_p;
    int num_ns, total_sum, i;
    read_name_file_to_memory(&ns_p, &num_ns, "resources/0022_names.txt");
    total_sum = 0;
    for(i = 0; i < num_ns; i++)
    {
        total_sum += ns_p[i].total_score;
    }
    free(ns_p);
    
    /* stop timing */
    end_cpu = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &end_real)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    /* calculate times in milliseconds */
    cpu_time_used_ms = 1000.0 * ((double)(end_cpu-start_cpu)) / CLOCKS_PER_SEC;
    if((ret = timespec_subtract(&real_time_used_ms, &end_real, &start_real)) != RETURN_SUCCESS)
        fprintf(stderr, "timespec_subtract returned %d, error = %s\n", ret, strerror(ret));
    

    ps->cpu_time_ms = cpu_time_used_ms;
    ps->real_time_ms = real_time_used_ms;

    ret = snprintf(buf, sizeof buf, 
            "The total of the names according to the rules: %d", total_sum);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: problem 022:");
        fprintf(stderr, "error: problem 022: snprintf error\n");
        return ENOMEM;
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "%d", total_sum);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 022:");
        fprintf(stderr, "error: problem 022: snprintf error\n");
        return ENOMEM;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return RETURN_SUCCESS;
}

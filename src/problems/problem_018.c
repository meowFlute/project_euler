#include "problem_018.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

typedef struct node_t {
    struct node_t* parent_left;
    struct node_t* parent_right;
    struct node_t* next_right;
    struct node_t* next_left;
    int value;
    int max_sum;
} node_t;

typedef struct triangle_t {
    node_t** rows_leftmost_nodes;
    int num_rows;
} triangle_t;

int maximum_path_sum(int* out, const char* t_path_str)
{
    int ret, i, max_sum;
    char* num_str = NULL;
    char* line_malloc = NULL;
    size_t line_len = 0;
    FILE* f_p;
    node_t* node_p = NULL;
    node_t* tmp_node_p = NULL;

    // read triangle.txt file into triangle_t
    if((f_p = fopen(t_path_str, "r")) == NULL)
    {
        ret = errno;
        perror("maximum_path_sum: error: fopen failure");
        return ret;
    }

    triangle_t* tri_p = (triangle_t*)malloc(sizeof(triangle_t));
    tri_p->num_rows = 0;
    tri_p->rows_leftmost_nodes = NULL; // first call to realloc will be malloc
    while((getline(&line_malloc, &line_len, f_p)) != -1)
    {
        tri_p->rows_leftmost_nodes = (node_t**)realloc(tri_p->rows_leftmost_nodes, 
                                                       sizeof(triangle_t*)*(tri_p->num_rows + 1));
        tri_p->rows_leftmost_nodes[tri_p->num_rows] = NULL; // set newly allocated spot to NULL
        num_str = strtok(line_malloc, " "); // init strtok
        do{
            // allocate node
            node_p = (node_t*)malloc(sizeof(node_t));

            // init node value
            node_p->value = atoi(num_str);

            // init left and right relationships
            if((tri_p->rows_leftmost_nodes[tri_p->num_rows]) == NULL)
            {
                node_p->next_left = NULL; // no left neighbor
                node_p->next_right = NULL; // no right neighbor yet
                tri_p->rows_leftmost_nodes[tri_p->num_rows] = node_p;
            }
            else
            {
                // traverse row to end
                tmp_node_p = tri_p->rows_leftmost_nodes[tri_p->num_rows];
                while(tmp_node_p->next_right != NULL)
                    tmp_node_p = tmp_node_p->next_right;

                // assign relationships
                node_p->next_left = tmp_node_p;
                tmp_node_p-> next_right = node_p;
                node_p->next_right = NULL;
            }

            // init parent relationships
            if(tri_p->num_rows > 0)
            {
                if(node_p->next_left == NULL)
                {
                    node_p->parent_left = NULL;
                    node_p->parent_right = tri_p->rows_leftmost_nodes[tri_p->num_rows-1];
                }
                else
                {
                    node_p->parent_left = node_p->next_left->parent_right;
                    node_p->parent_right = node_p->parent_left->next_right;
                }
                // calculate max sum from parent relationships
                if((node_p->parent_left != NULL) && (node_p->parent_right != NULL))
                {
                    if(node_p->parent_left->max_sum > node_p->parent_right->max_sum)
                    {
                        node_p->max_sum = node_p->value + node_p->parent_left->max_sum;
                    }
                    else // doesn't matter if they're equal or right is bigger, add right
                    {
                        node_p->max_sum = node_p->value + node_p->parent_right->max_sum;
                    }
                }
                else // one of the parents is null (edge node)
                {
                    if(node_p->parent_left != NULL)
                    {
                        node_p->max_sum = node_p->value + node_p->parent_left->max_sum;
                    }
                    else // node_p->parent_right != NULL
                    {
                        node_p->max_sum = node_p->value + node_p->parent_right->max_sum;
                    }
                }
            }
            else
            {
                node_p->parent_left = NULL;
                node_p->parent_right = NULL;
                node_p->max_sum = node_p->value;
            }
        }while((num_str = strtok(NULL, " ")) != NULL);
        tri_p->num_rows++;
    }

    if((ret = fclose(f_p)) != 0)
    {
        perror("maximum_path_sum: error: fclose failure");
        return errno;
    }
    free(line_malloc);
    
    // retrieve solution from last row of triangle
    max_sum = 0;
    node_p = tri_p->rows_leftmost_nodes[tri_p->num_rows-1];
    do{
        if(max_sum < node_p->max_sum)
            max_sum = node_p->max_sum;
        node_p = node_p->next_right;
    }while(node_p->next_right != NULL);

    // free all node_t containers
    for(i = (tri_p->num_rows-1); i >= 0; i--)
    {
        // start at the bottom of the triangle
        node_p = tri_p->rows_leftmost_nodes[i];
        while(node_p != NULL) // free all nodes in rows
        {
            tmp_node_p = node_p->next_right;
            free(node_p); // free from the node_t mallocs
            node_p = tmp_node_p;
        }
        tri_p->rows_leftmost_nodes[i] = NULL; // end of row clear
    }
    free(tri_p->rows_leftmost_nodes); // free pointer array from realloc
    free(tri_p); // free triangle_t container

    *out = max_sum;
    return RETURN_SUCCESS;
}

int problem_018(problem_solution *ps)
{
    clock_t start_cpu, end_cpu; 
    struct timespec start_real, end_real;
    double cpu_time_used_ms, real_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;

    ps->problem_number = 18U;
    ps->problem_statement = strdup("By starting at the top of the triangle"
            " below and moving to adjacent numbers on the row below, the maximum"
            " total from top to bottom is 23. [traingle omitted]."
            " Find the maximum total from top to bottom of the triangle"
            " located in resources/0018_triangle.txt");

    /* start timing */
    start_cpu = clock();
    if((ret = clock_gettime(CLOCK_MONOTONIC, &start_real)) != 0)
        fprintf(stderr, "clock_gettime returned -1, errno=%d, %s\n", errno, strerror(errno));

    //Solve problem 018
    int solution;
    if((ret = maximum_path_sum(&solution, "resources/0018_triangle.txt")) != RETURN_SUCCESS)
        fprintf(stderr, "problem 018: error: maximum_path_sum returned %d, %s\n", ret, strerror(ret));
    
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
            "maximum path of resources/0018_triangle.txt: %d", solution);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: problem 018:");
        fprintf(stderr, "error: problem 018: snprintf error\n");
        return ENOMEM;
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "%d", solution);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 018:");
        fprintf(stderr, "error: problem 018: snprintf error\n");
        return ENOMEM;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return RETURN_SUCCESS;
}

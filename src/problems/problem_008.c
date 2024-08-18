#include "problem_008.h" // export the global function
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <time.h>


/* PROBLEM 008
 * The four adjacent digits in the 1000-digit number that have the greatest 
 * product are: 
 *      9 * 9 * 8 * 9 = 5832
 * 
 * 73167176531330624919225119674426574742355349194934
 * 96983520312774506326239578318016984801869478851843
 * 85861560789112949495459501737958331952853208805511
 * 12540698747158523863050715693290963295227443043557
 * 66896648950445244523161731856403098711121722383113
 * 62229893423380308135336276614282806444486645238749
 * 30358907296290491560440772390713810515859307960866
 * 70172427121883998797908792274921901699720888093776
 * 65727333001053367881220235421809751254540594752243
 * 52584907711670556013604839586446706324415722155397
 * 53697817977846174064955149290862569321978468622482
 * 83972241375657056057490261407972968652414535100474
 * 82166370484403199890008895243450658541227588666881
 * 16427171479924442928230863465674813919123162824586
 * 17866458359124566529476545682848912883142607690042
 * 24219022671055626321111109370544217506941658960408
 * 07198403850962455444362981230987879927244284909188
 * 84580156166097919133875499200524063689912560717606
 * 05886116467109405077541002256983155200055935729725
 * 71636269561882670428252483600823257530420752963450
 * 
 * Find the thirteen adjacent digits in the 1000-digit number that have the 
 * greatest product. What is the value of this product?
 *  */
int problem_008(problem_solution * ps)
{
    clock_t start, end; 
    double cpu_time_used_ms;
    char buf[PE_SOLUTION_BUFFER_LEN];
    int ret;

    ps->problem_number = 8U;
    ps->problem_statement = strdup("The four adjacent digits in the 1000-digit"
            " number that have the greatest product are: 9 * 9 * 8 * 9 = 5832"
            " <number omitted>. Find the thirteen adjacent digits in the"
            " 1000-digit number that have the greatest product. What is the"
            " value of this product?");

    /* begin computation */
    start = clock();
    char n[] = "73167176531330624919225119674426574742355349194934\
96983520312774506326239578318016984801869478851843\
85861560789112949495459501737958331952853208805511\
12540698747158523863050715693290963295227443043557\
66896648950445244523161731856403098711121722383113\
62229893423380308135336276614282806444486645238749\
30358907296290491560440772390713810515859307960866\
172427121883998797908792274921901699720888093776\
65727333001053367881220235421809751254540594752243\
52584907711670556013604839586446706324415722155397\
53697817977846174064955149290862569321978468622482\
83972241375657056057490261407972968652414535100474\
82166370484403199890008895243450658541227588666881\
16427171479924442928230863465674813919123162824586\
17866458359124566529476545682848912883142607690042\
24219022671055626321111109370544217506941658960408\
07198403850962455444362981230987879927244284909188\
84580156166097919133875499200524063689912560717606\
05886116467109405077541002256983155200055935729725\
71636269561882670428252483600823257530420752963450";

    size_t i, j;
    uint64_t new_product, product; // 2^64-1 > 9^13 so this should be 
                                   // guaranteed
    product = 1UL;
    i = 0;
    while(n[i+12]) // will exit when n[i+12] == '\0' at the end of the string
    {
        // compute the product of the current and next 12 indicies
        new_product = 1UL;
        for(j = 0; j < 13; j++)
        {
            new_product *= (n[i+j] - '0');
        }

        // if the computed product is larger than the largest so far, store it
        if(new_product > product)
            product = new_product;

        // advance i
        i++;
    }
    /* end computation */
    end = clock();
    cpu_time_used_ms = 1000.0 * ((double)(end-start)) / CLOCKS_PER_SEC;
    ps->cpu_time_ms = cpu_time_used_ms;

    ret = snprintf(buf, sizeof buf, 
            "The largest 13-factor product in 1000 digit series: %" PRIu64, 
            product);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 008:");
        printf("Error: Problem 008: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    ret = snprintf(buf, sizeof buf, "%" PRIu64, product);
    if((ret == (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 008:");
        printf("Error: Problem 008: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);
    return EXIT_SUCCESS;
}

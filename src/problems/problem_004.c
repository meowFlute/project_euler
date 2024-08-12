#include "problem_004.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <gmp.h>
#include <time.h>
#include <errno.h>

/* this will be my first foray into using the GNU Multiprecision Arithmetic 
 * library GMP! It'll be overkill but I want to get comfortable with it now so
 * that it is there when I need it 
 * */

/* PROBLEM 3
 * A palindromic number reads the same both ways. The largest palindrome made 
 * from the product of two 2-digit numbers is
 * 
 * 9009 = 91 * 99
 *
 * Find the largest palindrome made from the product of two 3-digit numbers.
 * */
int problem_004(problem_solution * ps)
{
    clock_t start, end;
    double cpu_time_used_ms;
    ps->problem_number = 4U;
    ps->problem_statement = strdup("A palindromic number reads the same both"
            " ways. The largest palindrome made from the product of two 2-digit"
            " numbers is 9009 = 91 * 99. Find the largest palindrome made from"
            " the product of two 3-digit numbers.");

    start = clock();
    // initialize my gmp variables as described in the gmp docs
    mpz_t num1, num2, prod, test;
    mpz_inits(num1, num2, prod, test, NULL);
    unsigned long i, j;
    char test_str[] = "1234567"; // a container with enough room for our number
                                 // the number will be at most 6 digits, a null
                                 // terminator and a sign (8 bytes), which is
                                 // how much storage "1234567" statically 
                                 // allocates
    size_t test_str_len, front, back, k;
    _Bool is_palindrom = true;

    // to keep this solution simple we'll start at 999 * 999 and check the
    // product for being palindrome in two for loops until we find our
    // solution. Afte we find our first palindrome we can break the inner loop
    // every time our solutions start to get smaller
    for(i = 999UL; i > 0UL; i--)
    {
        for(j = 999UL; j > 0UL; j--)
        {
            // set the numbers manually
            mpz_set_ui(num1, i);
            mpz_set_ui(num2, j);

            // compute the product and put the answer in "test"
            mpz_mul(test, num1, num2);

            // check if it is greater than our current greatest palindrome so 
            // long as we've found one already
            if((mpz_cmp_ui(prod,0UL) != 0) && (mpz_cmp(prod,test) >= 0))
            {
                break;
            }

            // convert product to a string, we ignore the result on purpose
            // because it is just the pointer test_str
            (void)mpz_get_str(test_str, 10, test);
            
            // test for palindrome by getting the size and checking each to the
            // center
            test_str_len = strlen(test_str);
            front = 0; // first letter
            back = test_str_len - 1; // last letter
            is_palindrom = true; // assume it is a palindrome
            for(k = 0; k < (test_str_len/2); k++) //
            {
                if(test_str[front] != test_str[back])
                {
                    is_palindrom = false; // proof that it isn't
                    break;
                }
                front++;
                back--;
            }

            if(is_palindrom)
            {
                mpz_set(prod, test); // our new largest palindrome
            }
        }
    }

    end = clock();
    cpu_time_used_ms = 1000.0 * ((double)(end-start)) / CLOCKS_PER_SEC;
    ps->execution_time_ms = cpu_time_used_ms;
    
    char buf[PE_SOLUTION_BUFFER_LEN]; 
    int ret;
    /* natural language solution */
    ret = gmp_snprintf(buf, sizeof buf, 
            "The largest palindrome made from the product of two" 
            "3-digit numbers: %Zd", prod);
    if((ret >= (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 004:");
        printf("Error: Problem 004: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->natural_language_solution = strndup(buf, (sizeof buf) - 1);

    /* numeric string solution */
    ret = gmp_snprintf(buf, sizeof buf, "%Zd", prod);
    if((ret >= (int)(sizeof buf)) || (ret < 0))
    {
        perror("project_euler: Problem 004:");
        printf("Error: Problem 004: snprintf error\n");
        return EXIT_FAILURE;        
    }
    ps->numerical_solution = strndup(buf, (sizeof buf) - 1);

    // make sure to clear all the gmp variables
    mpz_clears(num1, num2, prod, test, NULL);

    return EXIT_SUCCESS;
}

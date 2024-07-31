#include <stdio.h>
#include "problem_009.h"

/* Problem 009:
 * A Pythagorean triplet is a set of three natural numbers, a < b < c, for 
 * which,
 *
 *      a^2 + b^2 = c^2
 * 
 * For example, 3^2 + 4^2 = 5^2.
 *
 * There exists exactly one Pythagorean triplet for which a + b + c = 1000
 * 
 * Find the product a*b*c
 * */
void problem_009()
{
    /* To start let's list out our constraints:
     *      1) a^2 + b^2 = c^2
     *      2) a + b + c = 1000
     *      3) we only care about the product a*b*c
     * */

    /* Search strategy:
     * we know from 2) above that:
     *      c = 1000 - a - b
     *      c^2 = (1000 - a - b)^2
     *      c^2 = a^2 + 2*a*b - 2000*a + b^2 - 2000*b + 1000000
     * 
     * we can insert that into 1):
     *      a^2 + b^2 = a^2 + b^2 + 2*a*b - 2000*a - 2000*b + 1000000
     *      0 = 2*a*b - 2000*a - 2000*b + 1000000
     *      2000*(a+b) - 2*a*b = 1000000
     *      a + b - (a*b)/1000 = 500
     *
     * what this tells us is three things we might use for our searching:
     *      1) a + b is exactly (a*b)/1000 greater than 500
     *      2) this means c is the same amount less than 500 (from constraint 2)
     *      3) since we're dealing with integers, a*b is a multiple of 1000
     *          a) this further means that a*b share the prime factorization of 
     *             1000: 2^3 * 5^3
     *          b) for example we could try (2^2*5 = 20) and (2*5^2 = 50)
     * 
     * Summarizing:
     *      1) a < b < c
     *      2) c is less than 500 by exactly ab/1000
     *      3) a + b is greater than 500 by exactly ab/1000
     *      4) a + b + c = 1000
     *      5) a^2 + b^2 = c^2
     *
     * Euclid's Formula:
     *      Euclid's fundamental formula can generate all pythagorean triples
     *      and may provide an easier search space
     *
     *      a = k * (m^2 - n^2), b = k*2*m*n, c = k*(m^2 + n^2)
     *      m > n > 0
     *
     *      so we can factor out the k and say:     
     *          m^2 - n^2 + 2*m*n + m^2 + n^2 = 1000/k
     *          2*m^2 + 2*m*n = 1000 / k
     *          m^2 + m*n = 500 / k
     *      in other words, we could just search for m and n such that 
     *      (m^2 + m*n) is any factor of 500. Taking the above summarized
     *      constraints we could say:
     *          1) m^2 - n^2 < 2*m*n < m^2*n^2
     *          2) m^2 + n^2 < 500
     *          3) m^2 + m*n is any factor of 500
     *
     *      knowing that 500 = 2^2 * 5^3 we could search as follows:
     *          - n = 500/m - m, is positive, is less than m
     *          - m is in the array [2, 4, 5, 10, 20] because 
     *                              [25, 50, 100, 125, 250 or 500] produce
     *                              negative n
     *          - m cannot be 2, 4, 5, 10 because n would be larger
     *          - m is 20
     *          - n is 5
     *      
     *      from this we know
     *          - a is a multiple k*200
     *          - b is a multiple k*375
     *          - c is a multiple k*425
     *
     *      we can see that with k = 1, a + b + c = 1000
     *
     *      so the solution is simply 200*375*425
     * */

    printf("Problem 009: for pythagorean triple where a+b+c=1000, "
            "a*b*c=%d\n", 200*375*425);
}

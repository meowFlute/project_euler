#include <stdio.h>
#include <inttypes.h>
#include "problem_011.h"

/* Problem 011:
 * In the 20 x 20 grid below, four numbers along a diagonal line have been 
 * marked with <>.
 *
 * 08 02 22 97 38 15 00 40 00 75 04 05 07 78 52 12 50 77 91 08
 * 49 49 99 40 17 81 18 57 60 87 17 40 98 43 69 48 04 56 62 00
 * 81 49 31 73 55 79 14 29 93 71 40 67 53 88 30 03 49 13 36 65
 * 52 70 95 23 04 60 11 42 69 24 68 56 01 32 56 71 37 02 36 91
 * 22 31 16 71 51 67 63 89 41 92 36 54 22 40 40 28 66 33 13 80
 * 24 47 32 60 99 03 45 02 44 75 33 53 78 36 84 20 35 17 12 50
 * 32 98 81 28 64 23 67 10<26>38 40 67 59 54 70 66 18 38 64 70
 * 67 26 20 68 02 62 12 20 95<63>94 39 63 08 40 91 66 49 94 21
 * 24 55 58 05 66 73 99 26 97 17<78>78 96 83 14 88 34 89 63 72
 * 21 36 23 09 75 00 76 44 20 45 35<14>00 61 33 97 34 31 33 95
 * 78 17 53 28 22 75 31 67 15 94 03 80 04 62 16 14 09 53 56 92
 * 16 39 05 42 96 35 31 47 55 58 88 24 00 17 54 24 36 29 85 57
 * 86 56 00 48 35 71 89 07 05 44 44 37 44 60 21 58 51 54 17 58
 * 19 80 81 68 05 94 47 69 28 73 92 13 86 52 17 77 04 89 55 40
 * 04 52 08 83 97 35 99 16 07 97 57 32 16 26 26 79 33 27 98 66
 * 88 36 68 87 57 62 20 72 03 46 33 67 46 55 12 32 63 93 53 69
 * 04 42 16 73 38 25 39 11 24 94 72 18 08 46 29 32 40 62 76 36
 * 20 69 36 41 72 30 23 88 34 62 99 69 82 67 59 85 74 04 36 16
 * 20 73 35 29 78 31 90 01 74 31 49 71 48 86 81 16 23 57 05 54
 * 01 70 54 71 83 51 54 69 16 92 33 48 61 43 52 01 89 19 67 48
 *
 * The product of these numbers is 26 x 63 x 78 x 14 = 1788696.
 *
 * What is the greatest product of four adjacent numbers in the same direction 
 * (up, down, left, right, or diagonally) in the 20 x 20 grid?
 * */
void problem_011()
{
    /* Solution Strategy:
     * I am going to assume up and down are the same things, as are left and
     * right, so we really have to search all possible combinations in the
     * following directions:
     *      1) Horizontal
     *      2) Vertical
     *      3) NW-SE Diagonal
     *      4) SW-NE Diagonal
     * 
     * This will be a trivial search in the horizontal and vertical directions
     * since every row/column has more than 4 numbers.
     *
     * The diagnoal are only slightly more complicated in that the last three
     * diagnoals in each corner have to be skipped.
     *
     * For storage, a 2D array is an easy choice, and since 2^7-1 > 99 we can
     * use anything char or greater. Since they're all positive and there are
     * some cases where operations are faster on unsigned integers we'll go
     * with uint8_t
     *
     * The product will be at most 99^4 = 96,059,601. log(99^4)/log(2) ~= 26.5,
     * and our product is guaranteed to be positive so a 32-bit unsigned number 
     * will work great.
     * */
    uint32_t largest_product = 1U;
    uint32_t product = 1U;
    uint8_t row, col;

#define HEIGHT 20
#define WIDTH 20

    const uint8_t num_terms = 4U;
    const uint8_t grid[WIDTH][HEIGHT] = {
        {8U,2U,22U,97U,38U,15U,0U,40U,0U,75U,4U,5U,7U,78U,52U,12U,50U,77U,91U,8U},
        {49U,49U,99U,40U,17U,81U,18U,57U,60U,87U,17U,40U,98U,43U,69U,48U,4U,56U,62U,0U},
        {81U,49U,31U,73U,55U,79U,14U,29U,93U,71U,40U,67U,53U,88U,30U,3U,49U,13U,36U,65U},
        {52U,70U,95U,23U,4U,60U,11U,42U,69U,24U,68U,56U,1U,32U,56U,71U,37U,2U,36U,91U},
        {22U,31U,16U,71U,51U,67U,63U,89U,41U,92U,36U,54U,22U,40U,40U,28U,66U,33U,13U,80U},
        {24U,47U,32U,60U,99U,3U,45U,2U,44U,75U,33U,53U,78U,36U,84U,20U,35U,17U,12U,50U},
        {32U,98U,81U,28U,64U,23U,67U,10U,26U,38U,40U,67U,59U,54U,70U,66U,18U,38U,64U,70U},
        {67U,26U,20U,68U,2U,62U,12U,20U,95U,63U,94U,39U,63U,8U,40U,91U,66U,49U,94U,21U},
        {24U,55U,58U,5U,66U,73U,99U,26U,97U,17U,78U,78U,96U,83U,14U,88U,34U,89U,63U,72U},
        {21U,36U,23U,9U,75U,0U,76U,44U,20U,45U,35U,14U,0U,61U,33U,97U,34U,31U,33U,95U},
        {78U,17U,53U,28U,22U,75U,31U,67U,15U,94U,3U,80U,4U,62U,16U,14U,9U,53U,56U,92U},
        {16U,39U,5U,42U,96U,35U,31U,47U,55U,58U,88U,24U,0U,17U,54U,24U,36U,29U,85U,57U},
        {86U,56U,0U,48U,35U,71U,89U,7U,5U,44U,44U,37U,44U,60U,21U,58U,51U,54U,17U,58U},
        {19U,80U,81U,68U,5U,94U,47U,69U,28U,73U,92U,13U,86U,52U,17U,77U,4U,89U,55U,40U},
        {4U,52U,8U,83U,97U,35U,99U,16U,7U,97U,57U,32U,16U,26U,26U,79U,33U,27U,98U,66U},
        {88U,36U,68U,87U,57U,62U,20U,72U,3U,46U,33U,67U,46U,55U,12U,32U,63U,93U,53U,69U},
        {4U,42U,16U,73U,38U,25U,39U,11U,24U,94U,72U,18U,8U,46U,29U,32U,40U,62U,76U,36U},
        {20U,69U,36U,41U,72U,30U,23U,88U,34U,62U,99U,69U,82U,67U,59U,85U,74U,4U,36U,16U},
        {20U,73U,35U,29U,78U,31U,90U,1U,74U,31U,49U,71U,48U,86U,81U,16U,23U,57U,5U,54U},
        {1U,70U,54U,71U,83U,51U,54U,69U,16U,92U,33U,48U,61U,43U,52U,1U,89U,19U,67U,48U},
    };
     
    /* Horizontal Search */
    for(row = 0U; row < HEIGHT; row++)
    {
        for(col = 0U; col < (WIDTH - num_terms); col++)
        {
            product = grid[row][col]*grid[row][col+1]*grid[row][col+2]
                        *grid[row][col+3];
            if(product > largest_product)
                largest_product = product;
        }
    }

    /* Vertical Search */
    for(col = 0U; col < WIDTH; col++)
    {
        for(row = 0U; row < (HEIGHT - num_terms); row++)
        {
            product = grid[row][col]*grid[row+1][col]*grid[row+2][col]
                        *grid[row+3][col];
            if(product > largest_product)
                largest_product = product;
        }
    }

    /* SW-NE Diagonal Search */
    for(row = 3; row < HEIGHT; row++)
    {
        for(col = 0; col < (WIDTH - num_terms); col++)
        {
            /* in this one unless you traverse diagonally (which seems like a
             * pain) you have to do a new product every time */
            product = grid[row][col]*grid[row-1][col+1]*grid[row-2][col+2]
                        *grid[row-3][col+3];
            if(product > largest_product)
                largest_product = product;
        }
    }

    /* NW-SE Diagonal Search */
    for(row = 0; row < (HEIGHT - num_terms); row++)
    {
        for(col = 0; col < (WIDTH - num_terms); col++)
        {
            product = grid[row][col]*grid[row+1][col+1]*grid[row+2][col+2]
                        *grid[row+3][col+3];
            if(product > largest_product)
                largest_product = product;
        }
    }

    printf("Problem 011: The largest product of 4 adjacent terms (crossword "
            "style): %" PRIu32 "\n", largest_product);
}

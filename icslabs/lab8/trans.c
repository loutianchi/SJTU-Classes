// name: 楼天驰
// ID: 522031910290

/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{    
    int i, j, ii, jj;
    int a1, a2, a3, a4, a5, a6, a7, a8;
    // special case for 32 * 32
    // Because eviction occurs when there are conflicts, we need to
    // minimize the conflicts. Handle a 8*8 block at a time can make
    // full use of the block (b = 5) and use just 11 local varables.
    if (N == 32 && M == 32) {
        for (i = 0; i < N / 8; i++)
            for (j = 0; j < M / 8; j++)
                for (ii = i * 8; ii < i * 8 + 8; ii++) {
                    a1 = A[ii][j * 8 + 0];
                    a2 = A[ii][j * 8 + 1];
                    a3 = A[ii][j * 8 + 2];
                    a4 = A[ii][j * 8 + 3];
                    a5 = A[ii][j * 8 + 4];
                    a6 = A[ii][j * 8 + 5];
                    a7 = A[ii][j * 8 + 6];
                    a8 = A[ii][j * 8 + 7];
                    B[j * 8 + 0][ii] = a1;
                    B[j * 8 + 1][ii] = a2;
                    B[j * 8 + 2][ii] = a3;
                    B[j * 8 + 3][ii] = a4;
                    B[j * 8 + 4][ii] = a5;
                    B[j * 8 + 5][ii] = a6;
                    B[j * 8 + 6][ii] = a7;
                    B[j * 8 + 7][ii] = a8;
                }
    }
    // special case for 64 * 64
    // Because there are only 4 set for array A and 4 set for array B
    // can exist at the same time, I used a trick to reduce misses.
    // The key point is to make most use of 32 blocks(8 integers) of a
    // line in memory.
    // Handle a 8 * 8 block at a time like before.
    // Because we can modify array B as we like, the first 4 lines for
    // array A is transposed by 4 * 4 blocks and stored in first 4 lines
    // in array B. The next step copy the up/right 4*4 block to local
    // variables, and fill the up/right block with right numbers.
    // Then fill the 4 * 8 block with local variables and array A.
    // Through the above operations, we can reduce the times of visiting
    // the up/right zone of array A.
    else if (N == 64 && M == 64) {
        for (i = 0; i < N / 8; i++)
            for (j = 0; j < M / 8; j++) {
                for (ii = 0; ii < 4; ii++) {
                    a1 = A[i * 8 + ii][j * 8 + 0];
                    a2 = A[i * 8 + ii][j * 8 + 1];
                    a3 = A[i * 8 + ii][j * 8 + 2];
                    a4 = A[i * 8 + ii][j * 8 + 3];
                    a5 = A[i * 8 + ii][j * 8 + 4];
                    a6 = A[i * 8 + ii][j * 8 + 5];
                    a7 = A[i * 8 + ii][j * 8 + 6];
                    a8 = A[i * 8 + ii][j * 8 + 7];
                    B[j * 8 + 0][i * 8 + ii] = a1;
                    B[j * 8 + 1][i * 8 + ii] = a2;
                    B[j * 8 + 2][i * 8 + ii] = a3;
                    B[j * 8 + 3][i * 8 + ii] = a4;
                    B[j * 8 + 0][i * 8 + ii + 4] = a5;
                    B[j * 8 + 1][i * 8 + ii + 4] = a6;
                    B[j * 8 + 2][i * 8 + ii + 4] = a7;
                    B[j * 8 + 3][i * 8 + ii + 4] = a8;
                }
                for (ii = 4; ii < 8; ii++) {
                    a1 = B[j * 8 + ii - 4][i * 8 + 4];
                    a2 = B[j * 8 + ii - 4][i * 8 + 5];
                    a3 = B[j * 8 + ii - 4][i * 8 + 6];
                    a4 = B[j * 8 + ii - 4][i * 8 + 7];

                    B[j * 8 + ii - 4][i * 8 + 4] = A[i * 8 + 4][j * 8 + ii - 4];
                    B[j * 8 + ii - 4][i * 8 + 5] = A[i * 8 + 5][j * 8 + ii - 4];
                    B[j * 8 + ii - 4][i * 8 + 6] = A[i * 8 + 6][j * 8 + ii - 4];
                    B[j * 8 + ii - 4][i * 8 + 7] = A[i * 8 + 7][j * 8 + ii - 4];

                    B[j * 8 + ii][i * 8 + 0] = a1;
                    B[j * 8 + ii][i * 8 + 1] = a2;
                    B[j * 8 + ii][i * 8 + 2] = a3;
                    B[j * 8 + ii][i * 8 + 3] = a4;

                    B[j * 8 + ii][i * 8 + 4] = A[i * 8 + 4][j * 8 + ii];
                    B[j * 8 + ii][i * 8 + 5] = A[i * 8 + 5][j * 8 + ii];
                    B[j * 8 + ii][i * 8 + 6] = A[i * 8 + 6][j * 8 + ii];
                    B[j * 8 + ii][i * 8 + 7] = A[i * 8 + 7][j * 8 + ii];

                }
            }
    }
    // 61 * 67: Two prime numbers. The set numbers are unlikely to repeat.
    // So make full use of the 32 sets, so split into 16 * 16 block.
    // Just be careful with the order of row and column, and the boundaries.
    else {
        for (i = 0; i < (N + 15) / 16; i++)
            for (j = 0; j < (M + 15) / 16; j++) {
                for (ii = i * 16; ii < i * 16 + 16 && ii < N; ii++)
                    for (jj = j * 16; jj < j * 16 + 16 && jj < M; jj++)
                        B[jj][ii] = A[ii][jj];
            }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}


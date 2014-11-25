/*
 * uarray2b.c
 *
 * This is the implementation of uarray2b.c
 * by Mengtian Li and Mingzhe Li, 10/3/14
 *
 */

#include "assert.h"
#include "stdlib.h"
#include "mem.h"
#include "uarray2b.h"
#include "math.h"
#include "uarray.h"
#include "uarray2.h"
#include "stdio.h"
#define T UArray2b_T

struct T {
        int width, height;
        int size;
        int blocksize;
        UArray2_T blocks; 
};

void initialize_block(int i, int j, UArray2_T a, void* data, void* p2);

T UArray2b_new (int width, int height, int size, int blocksize)
{
        assert(width >= 0 && height >= 0 && size >= 0 && blocksize >= 0 );
        T arrayb;
        NEW(arrayb);
        arrayb->width = width;
        arrayb->height = height;
        arrayb->size = size;
        arrayb->blocksize = blocksize;
        int blocks_width = (int)ceil((double)width / (double)blocksize);
        int blocks_height = (int)ceil((double)height / (double)blocksize);
        arrayb->blocks = UArray2_new(blocks_width, blocks_height, 
                                   sizeof(UArray_T));
        UArray2_map_row_major(arrayb->blocks, initialize_block, arrayb); 
        return arrayb;
}

void initialize_block(int i, int j, UArray2_T a, void* data, void* p2)
{
        (void)i;
        (void)j;
        (void)a;
        *(UArray_T*)data = UArray_new(((T) p2)->blocksize * ((T)p2)->blocksize, 
                                      ((T)p2)->size);
}

T UArray2b_new_64K_block(int width, int height, int size)
{
        int blocksize = sqrt((double)(65536 / size));
        return UArray2b_new(width, height, size, blocksize);    
}

void UArray2b_free(T *array2b)
{
        assert(array2b && *array2b);
        UArray2_free(&(*array2b)->blocks);
        FREE(*array2b); 
}

int UArray2b_width (T array2b)
{
        assert(array2b);
        return array2b->width;
}

int UArray2b_height(T array2b)
{
        assert(array2b);
        return array2b->height;
}

int UArray2b_size (T array2b)
{
        assert(array2b);
        return array2b->size;
}

int UArray2b_blocksize(T array2b)
{
        assert(array2b);
        return array2b->blocksize;
}

void *UArray2b_at(T array2b, int i, int j)
{
        assert(array2b);
        void *to_rtn;
        UArray_T tmp;
        int blocksize = array2b->blocksize;
        tmp = *(UArray_T*)UArray2_at(array2b->blocks, i / blocksize, 
                                     j / blocksize);
        to_rtn = UArray_at(tmp, blocksize * (i % blocksize) + (j % blocksize));
        if (to_rtn != NULL)
                return to_rtn;
        else 
                exit(1);
}

void UArray2b_map(T array2b,
                  void apply(int i, int j, T array2b, void *elem, 
                             void *cl), void *cl)
{
        assert(array2b);
        int h = array2b->height;
        int w = array2b->width;
        int b = array2b->blocksize;
        int blockrow;
        int blockcol;
        /* walk through the matrix of blocks */
        for (int row = 0; row < h; row = row + b) {
                for (int col = 0; col < w; col = col + b) {
                        blockrow = row + b;
                        /* walk through each cell in a block*/
                        for (int i = row; i < blockrow && i < h; i++) {
                                blockcol = col + b;
                                for (int j = col; j < blockcol && j < w; j++) {
                                        apply(j, i, array2b,
                                              UArray2b_at(array2b, j, i),
                                              cl);
                                }
                        }
                }
        }
}

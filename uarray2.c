/*
 * uarray2
 * This is an implementation of UArray2_T
 * Two-Dimensional, Polymorphic, Unboxed Arrays
 * by Mingzhe Li and Mengtian Li, 9/17/14
 *
 */

#include <uarray.h>
#include "uarray2.h"
#include <assert.h>
#include <stdio.h>
#include <mem.h>
#include <stdlib.h>

#define T UArray2_T

/* We defined the struct with width and height based on an unboxed array */
struct T {
        int width;
        int height;
        UArray_T arr; 
};


/* takes in the width, height, and size for each element,  
 * initializes the array, and returns it.
 */
T UArray2_new(int width, int height, int size)
{
        T toRtn = malloc(sizeof(*toRtn));
        assert(toRtn != NULL);

        toRtn->arr = UArray_new(width * height, size);
        toRtn->width = width;
        toRtn->height = height;
        return toRtn;
}

/* calls UArray_free() to free the memory */
void UArray2_free(T *array)
{
        UArray_free(&(*array)->arr);
        FREE(*array);
}

/* returns the width of the 2D array */
int UArray2_width(T array)
{
        return array->width;
}

/* returns the height of the 2D array */
int UArray2_height(T array)
{
        return array->height;
}

/* returns the size of each element in the 2D array */
int UArray2_size(T array)
{
        return UArray_size(array->arr);
}


/* recalls UArray_at() to return a pointer to an element of the width and
 * height in the 2D array 
 */
void *UArray2_at(T array, int row, int col)
{
        return UArray_at(array->arr, col * (array->width) + row);
}


/* Similar to the mapping function in Hanson's implementation, each time 
 * this function loops through the 2D array, it calls an apply function
 * which takes in the row, col, and element of that position.
 *
 * The only difference between _row_major() and _col_major() is choosing the 
 * priority between row or column.  This is the column-major version of the
 * mapping function
 */
void UArray2_map_col_major(T array, 
                           void apply(int i, int j, T a, void *data, void *p2),
                           void *cl)
{
        assert(array->arr);
        for (int row = 0; row < array->width; row++) {
                for (int col = 0; col < array->height; col++) {
                        apply(row, col, array, UArray2_at(array, row, col), cl);
                }
        }   
                
}

/* As explained above, this is the row-major version of the mapping function */
void UArray2_map_row_major(T array, 
                           void apply(int i, int j, T a, void *data, void *p2),
                           void *cl)
{
        assert(array->arr);
        for (int col = 0; col < array->height; col++) {
                for (int row = 0; row < array->width; row++) {
                        apply(row, col, array, UArray2_at(array, row, col), cl);
                }
        }   
                
}

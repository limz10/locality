/*
 * ppmtrans.c
 *
 * This is ppmtrans for part C of the locality assignment
 * by Mingzhe Li and Mengtian Li, 10/3/14
 *
 * Summary: This program takes in a ppm image file, and various arguments
 * as modification options.  It can rotate/flip/transpose an image by 
 * certain angle/orientation, using methods built on either UArray2 
 * or blocked UArray2b representation.
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assert.h"
#include "a2methods.h"
#include <mem.h>
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"

static const int HORIZONTAL = 1;
static const int VERTICAL = 2;
static const int TRANSPOSE = 3;

static void usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                "[-{row,col,block}-major] [filename]\n",
                progname);
        exit(1);
}

typedef struct {
        A2Methods_T methods;
        A2Methods_UArray2 uarray2;
} Info;





/* applyfunc for rotate 90 */
void rotate_90(int i, int j, A2Methods_UArray2 uarray, void  *data, void *cl)
{
        Info *info = cl;
        A2Methods_T methods = info->methods;
        A2Methods_UArray2 product = info->uarray2;
        int h = methods->height(uarray);

        Pnm_rgb tmp; 
        tmp = methods->at(product, h - j - 1, i);
        *tmp = *(Pnm_rgb)data;
}

/* applyfunc for rotate 180 */
void rotate_180(int i, int j, A2Methods_UArray2 uarray, void  *data, void *cl)
{
        Info *info = cl;
        A2Methods_T methods = info->methods;
        A2Methods_UArray2 product = info->uarray2;
        int h = methods->height(uarray);
        int w = methods->width(uarray);


        Pnm_rgb tmp;
        tmp = methods->at(product, w - i - 1, h - j - 1);
        *tmp = *(Pnm_rgb)data;
}

/* applyfunc for rotate 270 */
void rotate_270(int i, int j, A2Methods_UArray2 uarray, void  *data, void *cl)
{
        Info *info = cl;
        A2Methods_T methods = info->methods;
        A2Methods_UArray2 product = info->uarray2;
        int w = methods->width(uarray);

        Pnm_rgb tmp; 
        tmp = methods->at(product, j, w - i - 1);
        *tmp = *(Pnm_rgb)data;
}

/* applyfunc for flip vertical */
void flip_vert(int i, int j, A2Methods_UArray2 uarray, void  *data, void *cl)
{
        Info *info = cl;
        A2Methods_T methods = info->methods;
        A2Methods_UArray2 product = info->uarray2;
        int h = methods->height(uarray);

        Pnm_rgb tmp; 
        tmp = methods->at(product, i, h - j - 1);
        *tmp = *(Pnm_rgb)data;
}

/* applyfunc for flip horizontal */
void flip_hori(int i, int j, A2Methods_UArray2 uarray, void  *data, void *cl)
{
        Info *info = cl;
        A2Methods_T methods = info->methods;
        A2Methods_UArray2 product = info->uarray2;
        int w = methods->width(uarray);

        Pnm_rgb tmp; 
        tmp = methods->at(product, w - i - 1, j);
        *tmp = *(Pnm_rgb)data;
}

/* applyfunc for transpose */
void transpose(int i, int j, A2Methods_UArray2 uarray, void  *data, void *cl)
{
        (void)uarray;
        Info *info = cl;
        A2Methods_T methods = info->methods;
        A2Methods_UArray2 product = info->uarray2;

        Pnm_rgb tmp; 
        tmp = methods->at(product, j, i);
        *tmp = *(Pnm_rgb)data;
}

/* this function serves as the umbrella function of the modifications
 * it takes in a ppm image, A2Methods suite, A2Methods mapping function
 * and a signal mode for operations to perform
 * provided a valid input, it will return a UArray2 containing information
 * of the modified image
 *
 * It initializes an Info struct to help pass objects to other functions
 */
A2Methods_UArray2 transform(Pnm_ppm to_trans, A2Methods_T methods,
                            A2Methods_mapfun *map, int mode)
{
        Info *info = malloc(sizeof(Info));
        info->methods = methods;
        A2Methods_UArray2 to_rtn;

        if (mode == 0) {
                FREE(info);
                return to_trans->pixels;
        }
        else if (mode == 90) {
                info->uarray2 = methods->new(to_trans->height, 
                                             to_trans->width, 
                                             sizeof(Pnm_rgb) * 2);
                map(to_trans->pixels, rotate_90, info);
                to_rtn = info->uarray2;
                FREE(info);
                return to_rtn;
        }
        else if (mode == 180) {
                info->uarray2 = methods->new(to_trans->width, 
                                             to_trans->height,
                                             sizeof(Pnm_rgb) * 2);

                map(to_trans->pixels, rotate_180, info);
                to_rtn = info->uarray2;
                FREE(info);                
                return to_rtn;
        }
        else if (mode == 270) {
                info->uarray2 = methods->new(to_trans->height, 
                                             to_trans->width,
                                             sizeof(Pnm_rgb) * 2);
                map(to_trans->pixels, rotate_270, info);
                to_rtn = info->uarray2;
                FREE(info);
                return to_rtn;
        } 
        else if (mode == HORIZONTAL) {
                info->uarray2 = methods->new(to_trans->width,
                                             to_trans->height,
                                             sizeof(Pnm_rgb) * 2);
                map(to_trans->pixels, flip_hori, info);
                to_rtn = info->uarray2;
                FREE(info);
                return to_rtn;
        } 
        else if (mode == VERTICAL) {
                info->uarray2 = methods->new(to_trans->width,
                                             to_trans->height,
                                             sizeof(Pnm_rgb) * 2);
                map(to_trans->pixels, flip_vert, info);
                to_rtn = info->uarray2;
                FREE(info);
                return to_rtn;
        } 
        else if (mode == TRANSPOSE) {
                info->uarray2 = methods->new(to_trans->height,
                                             to_trans->width,
                                             sizeof(Pnm_rgb) * 2);
                map(to_trans->pixels, transpose, info);
                to_rtn = info->uarray2;
                FREE(info);
                return to_rtn;
        }
        exit(1);
}

/* this function prints the output to stdout
 * it takes in the original image in ppm format for headers,
 * and a UArray2 of info of themodified image
 */
void print_output(A2Methods_UArray2 result, Pnm_ppm to_trans)
{
        to_trans->methods->free(&(to_trans->pixels));
        to_trans->pixels = result;
        to_trans->width = to_trans->methods->width(result);
        to_trans->height = to_trans->methods->height(result);
        Pnm_ppmwrite(stdout, to_trans);
}


int main(int argc, char *argv[])
{
        /* mode indicates which modification to be used */
        int mode = -1;
        char* flip = " ";
        FILE *fp;
        int i;
        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods);
        /* default to best map */
        A2Methods_mapfun *map = methods->map_default;
        assert(map);

        int file_from_stdin = 1;

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
                methods = (METHODS);                            \
                assert(methods != NULL);                        \
                map = methods->MAP;                             \
                if (map == NULL) {                              \
                        fprintf(stderr, "%s does not support "  \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                        exit(1);                                \
                }                                               \
        } while (0)

        for (i = 1; i < argc; i++) {
                if (!strcmp(argv[i], "-row-major")) {
                        SET_METHODS(uarray2_methods_plain, map_row_major,
                                    "row-major");
                } else if (!strcmp(argv[i], "-col-major")) {
                        SET_METHODS(uarray2_methods_plain, map_col_major,
                                    "column-major");
                } else if (!strcmp(argv[i], "-block-major")) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } 
                else if (!strcmp(argv[i], "-rotate")) {
                        if (!(i + 1 < argc)) { /* no rotate value */ 
                                usage(argv[0]);
                        }
                        char *endptr;
                        mode = strtol(argv[++i], &endptr, 10);
                        if (!(mode == 0 || mode == 90
                                || mode == 180 || mode == 270)) {
                                fprintf(stderr, "Rotation must be "
                                        "0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) { /* Not a number */
                                usage(argv[0]);
                        }
                } 
                
                else if (!strcmp(argv[i], "-flip")) {
                        if (!((i + 1) < argc)) {
                                usage(argv[0]);
                        }       
                        flip = argv[++i];
                        if (!strcmp(flip, "horizontal")){
                                mode = HORIZONTAL;
                        } else if (!strcmp(flip, "vertical")){
                                mode = VERTICAL;
                        } else {
                                usage(argv[0]);
                        }
                } 

                else if(!strcmp(argv[i], "-transpose")){
                        mode = TRANSPOSE;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option ’%s’\n", argv[0],
                                argv[i]);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else if (i == argc - 1){
                        fp = fopen(argv[i], "rb");
                        file_from_stdin = 0;
                        if (fp == NULL) {
                                fprintf(stderr, "file cannot be opened");
                                fclose(fp);
                                exit(1);
                        }
                } else {
                        break;
                }
        }
        if (file_from_stdin) {
                fp = stdin;
        }

        assert (mode != -1);
                
        Pnm_ppm to_trans = Pnm_ppmread(fp, methods);

        A2Methods_UArray2 to_print = transform(to_trans, methods, map, mode);

        if (mode != 0) {
                print_output(to_print, to_trans);
                Pnm_ppmfree(&to_trans);

        } else { /* image unchanged */
                Pnm_ppmwrite(stdout, to_trans);
                Pnm_ppmfree(&to_trans);
        }
                
        fclose(fp);
        exit(0);
}

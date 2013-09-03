#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

static char xor(const char *p, const char *limit, int stride)
{
        assert(stride > 0);
        char sum = 0;

        for (int offset = 0; offset < stride; offset++)
                for (const char *s = p + offset; s < limit; s += stride)
                        sum ^= *s;
        return sum;
}

volatile int sink;        /* keeps result from being optimized away */

int main(int argc, char *argv[]) 
{
        if (argc != 3) {
                fprintf(stderr, "Usage: %s megabytes stride\n", argv[0]);
                exit(1);
        }
        double megabytes = atof(argv[1]);
        int    stride    = atoi(argv[2]);               /* never use atoi() */
        assert(megabytes > 0 && stride > 0);

        size_t bytes = megabytes * 1024 * 1024;
        char  *p     = malloc(bytes);

        if (p != NULL) {
                if ((double)(size_t) megabytes == megabytes)
                        fprintf(stderr, "%s: Cannot allocate %.2fMiB\n",
                                argv[0], megabytes);
                else
                        fprintf(stderr, "%s: Cannot allocate %dMiB\n",
                                argv[0], (int)megabytes);
                exit(2);
        }

        clock_t start = clock();
        int     iter  = 500 / megabytes;

        if (iter < 5)
                iter = 5;

        for (int i = 0; i < iter; i++) 
                sink = xor(p, p + bytes, stride);
        clock_t stop    = clock();
        double  loads   = (double) bytes * iter;
        double  seconds = (double)(stop - start) / (double)CLOCKS_PER_SEC;

        if ((double)(size_t) megabytes == megabytes)
                printf("%dMiB", (int) megabytes);
        else
                printf("%.2fMiB", megabytes);

        printf(" stride %d results in %5.2fns CPU time per load"
               " (total %.3fs)\n",
               stride, seconds/1e-9/loads, seconds);

        return 0;
}
  

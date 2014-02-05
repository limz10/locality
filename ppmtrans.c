#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"

static void
usage(const char *progname)
{
	fprintf(stderr, "Usage: %s [-rotate <angle>] "
		"[-{row,col,block}-major] [filename]\n",
		progname);
	exit(1);
}

int main(int argc, char *argv[])
{
	int rotation = 0;
	int i;


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
		} else if (!strcmp(argv[i], "-rotate")) {
			if (!(i + 1 < argc)) {      /* no rotate value */
				usage(argv[0]);
			}
			char *endptr;
			rotation = strtol(argv[++i], &endptr, 10);
			if (!(rotation == 0 || rotation == 90
			      || rotation == 180 || rotation == 270)) {
				fprintf(stderr, "Rotation must be "
					"0, 90 180 or 270\n");
				usage(argv[0]);
			}
			if (!(*endptr == '\0')) {    /* Not a number */
				usage(argv[0]);
			}
		} else if (*argv[i] == '-') {
			fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
				argv[i]);
		} else if (argc - i > 1) {
			fprintf(stderr, "Too many arguments\n");
			usage(argv[0]);
		} else {
			break;
		}
	}
	printf("It worked\n");
	assert(0);		// the rest of this function is not yet implemented
}

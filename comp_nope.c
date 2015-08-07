#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comp_nope.h"

extern FILE * compress_nope(FILE *orig, filesize_t *orig_size, filesize_t *archf_size)
{
	FILE* archf = tmpfile();
	rewind(orig);
	*archf_size = *orig_size = 0;
	byte_t c;
	while (!feof(orig))
	{
		if (fscanf(orig, "%c", &c) <= 0) break;
		fprintf(archf, "%c", c);
		++*orig_size;
	}
	*archf_size = *orig_size;
	return archf;
}

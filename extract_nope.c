#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extract_nope.h"

extern FILE * extract_nope(FILE *archf, filesize_t orig_size)
{
	FILE *orig = tmpfile();
	filesize_t i;
	byte_t c;
	for(i = 0; i < orig_size; i++)
	{
		fscanf(archf, "%c", &c);
		fprintf(orig, "%c", c);
	}
	return orig;
}

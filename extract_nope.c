#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extract_nope.h"

extern void extract_nope(FILE *archf, filesize_t orig_size, FILE *orig)
{
	byte_t c;
	for(filesize_t i = 0; i < orig_size; i++)
	{
		fscanf(archf, "%c", &c);
		fprintf(orig, "%c", c);
	}
}

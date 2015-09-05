#ifndef COMPRESS_H
#define COMPRESS_H

#include <stdlib.h>
#include "types.h"

typedef
	struct cano_huff_t
	{
		byte_t c, length;
		unsigned long long code;
	} cano_huff_t;

extern void compress_huffman(FILE *orig, FILE *archf, filesize_t *original_size, filesize_t *compressed_size);

extern void generate_codes(cano_huff_t *);

extern int capacity(int a);

#endif

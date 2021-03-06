#ifndef COMPRESS_H
#define COMPRESS_H

#include <stdlib.h>

#define CHARS_NUM (256)
#define ASCII_0 (48)

typedef
	struct cano_huff_t
	{
		unsigned char c, length;
		unsigned long long code;
	} cano_huff_t;

extern FILE * compress_huffman(FILE *, unsigned int *, unsigned int *);

extern void generate_codes(cano_huff_t *);

extern int capacity(int a);

#endif

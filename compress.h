#ifndef COMPRESS_H
#define COMPRESS_H

#include <stdlib.h>

#define CHARS_NUM (256)
#define ASCII_0 (48)

typedef
	struct cano_huff_t
	{
		unsigned char c;
		int length;
		unsigned int code;
	} cano_huff_t;

extern FILE * compress_huffman(FILE *, char ArchiveName[200]);

extern void generate_codes(cano_huff_t *);

#endif

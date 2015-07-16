#ifndef COMPRESS_LZW_H
#define COMPRESS_LZW_H

#include <stdio.h>
#include <stdlib.h>

typedef
	struct dictionary_t
	{
		int size;
		long long *code;
		int *code_len;
		char **word;
		int *word_len;
	} dictionary_t;
		
typedef
	struct string_t
	{
		char chars[1000];
		int length;
	} string_t;

extern void compress_lzw(FILE *orig, FILE *archf, unsigned int *orig_size, unsigned int *archf_size);

#endif

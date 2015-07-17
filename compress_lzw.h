#ifndef COMPRESS_LZW_H
#define COMPRESS_LZW_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "compress.h"

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

extern string_t * create_str();

extern dictionary_t * create_dict();

extern void assign(string_t *, char);

extern void compress_lzw(FILE *, FILE *, unsigned int *, unsigned int *);

extern void add_to_dictionary(dictionary_t *, string_t *);

extern int dict_str_id(dictionary_t *, string_t *);

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compress_lzw.h"

int current_code_len;

extern dictionary_t * create_dict()
{
	dictionary_t *new_dict = (dictionary_t*)malloc(sizeof(dictionary_t));
	new_dict->size = 0;
	new_dict->code = NULL;
	new_dict->word = NULL;
	new_dict->word_len = NULL;
	return new_dict;
}

inline static int is_two_degree(int a)
{
	return !(a&(a-1));
}

extern int add_to_dictionary(dictionary_t *dict, string_t *str)
{
	int size = ++dict->size;
	dict->code     = (long long*)realloc(dict->code,     size * sizeof(long long));
	dict->word     = (char**)    realloc(dict->word,     size * sizeof(char*));
	dict->word_len = (int*)      realloc(dict->word_len, size * sizeof(int));

	dict->code    [size - 1] = size - 1;
	dict->word    [size - 1] = (char*)malloc(str->length * sizeof(char));
	memcpy(dict->word[size - 1], str->chars, str->length * sizeof(char));
	dict->word_len[size - 1] = str->length;
	return is_two_degree(size);
}

extern int dict_str_id(dictionary_t *dict, string_t *str)
{
	int i, j, equal = 1;
	for(i = 0; i < dict->size; i++)
	{
		equal = dict->word_len[i] == str->length;
		for(j = 0; j < str->length && equal; j++)
			equal = dict->word[i][j] == str->chars[j];
		if (equal) return i;
	}
	return -1;
}

extern void compress_lzw(FILE *orig, FILE *archf, filesize_t *orig_size, filesize_t *archf_size)
{
	*orig_size = *archf_size = 0;
	current_code_len = 8;
	dictionary_t *dict = create_dict();
	string_t     *str  = create_str();
	int i;
	for(i = 0; i < CHARS_NUM; i++)
	{
		assign(str, (char)i);
		add_to_dictionary(dict, str);
	}
	++current_code_len;
	int prev_id, t, k = 7, code, bit;
	char c, prin_c = (char)0;
	fscanf(orig, "%c", &c);
	for(i = 0; (char)i != c; prev_id = ++i);
	rewind(orig);
	str->length = 0;
	while (!feof(orig))
	{
		if (fscanf(orig, "%c", &c) <= 0) break;
		append(str, c);
		t = dict_str_id(dict, str);
		if (t == -1)
		{
			if (add_to_dictionary(dict, str)) ++current_code_len;
			code = dict->code[prev_id];
			for(i = current_code_len - 1; i >= 0; i--)
			{
				bit = !!(code & (1 << i));
				prin_c = prin_c | (bit << k);
				if (--k == -1)
				{
					fprintf(archf, "%c", prin_c);
					prin_c = (char)0;
					k = 7;
					++*archf_size;
				}
			}
			assign(str, c);
			for(i = 0, prev_id = 0; (char)i != c; prev_id = ++i);
		}
		else
			prev_id = t;
	}
	if (k < 7)
	{
		fprintf(archf, "%c", prin_c);
		++*archf_size;
	}
}

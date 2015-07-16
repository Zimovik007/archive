#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compress_lzw.h"
	
static dictionary_t * create_dict()
{
	dictionary_t *new_dict = (dictionary_t*)malloc(sizeof(dictionary_t));
	new_dict->size = 0;
	new_dict->code = NULL;
	new_dict->length_code = NULL;
	new_dict->word = NULL;
	new_dict->word_len = NULL;
	return new_dict;	
}

static string_t * create_str()
{
	string_t *new_str = (string_t*)malloc(sizeof(string_t));
	new_str->length = 0;
	return new_str;	
}

static void append(string_t *str, char c)
{
	int length = ++str->length;
	str->chars[length] = '\0';
	str->chars[length - 1] = c;	
}

static void assign(string_t *str, char c)
{
	str->length = 1;
	str->chars[0] = c;
}

static void add_to_dictionary(dictionary_t *dict, string_t *str)
{
	int size = ++dict->size;
	dict->code = (long long*)realloc(dict->code, size * sizeof(long long));
	dict->length_code = (int*)realloc(dict->length_code, size * sizeof(int));
	dict->word = (char**)realloc(dict->word, size * sizeof(char*));
	dict->word_len = (int*)realloc(dict->word_len, size * sizeof(int));
	
	dict->code[size - 1] = size - 1;
	//dict->length_code[size - 1] = ?;
	memcpy(dict->word[size - 1], str->chars, str->length * sizeof(char));
	dict->word_len[size - 1] = str->length;
}

static int str_id(dictionary_t *dict, string_t *str)
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

extern void compress_lzw(FILE *orig, FILE *archf, unsigned int *orig_size, unsigned int *archf_size)
{
	dictionary_t *dict = create_dict();
	string_t *str = create_str();
	int i;
	for(i = 0; i < 256; i++)
	{
		assign(str, (char)i);
		add_to_dictionary(dict, str);		
	}
	
	
	
	
}

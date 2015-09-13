#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extract_lzw.h"
#include "compress_lzw.h"

int current_code_len;

static long long take_code(FILE *archf)
{
	static int  k = 7;
	static char c;
	long long code = 0;
	if (k == 7)
		if (fscanf(archf, "%c", &c) <= 0) return code;
	for(int i = current_code_len - 1; i >= 0; i--)
	{
		int bit = !!(c & (1 << k));
		code = code | (bit << i);
		if (--k == -1)
		{
			k = 7;
			if (i != 0)
				if (fscanf(archf, "%c", &c) <= 0) return code;
		}
	}
	return code;
}

static void assign_s(string_t *str, char *chars, int len)
{
	memcpy(str->chars, chars, len * sizeof(char));
	str->length = len;
}

static string_t * concat_str(string_t *s1, string_t *s2)
{
	string_t *new_str = create_str();
	memcpy(new_str->chars,              s1->chars, s1->length * sizeof(char));
	memcpy(new_str->chars + s1->length, s2->chars, s2->length * sizeof(char));
	new_str->length = s1->length + s2->length;
	return new_str;
}

static void print_word(FILE *orig, dictionary_t *dict, long long code)
{
	fwrite(dict->word[code], sizeof(char), dict->word_len[code], orig);
}

extern void extract_lzw(FILE *archf, filesize_t origsize, FILE *orig)
{
	current_code_len = 8;
	dictionary_t *dict = create_dict();
	string_t     *str  = create_str();
	for(int i = 0; i < CHARS_NUM; i++)
	{
		assign(str, (char)i);
		add_to_dictionary(dict, str);
	}
	++current_code_len;
	string_t *str1 = create_str(), *str2 = create_str();
	while(!feof(archf))
	{
		long long code = take_code(archf);
		print_word(orig, dict, code);
		assign_s(str1, dict->word[code], dict->word_len[code]);
		str1 = concat_str(str2, str1);
		if (dict_str_id(dict, str1) == -1)
			add_to_dictionary(dict, str1);
		assign_s(str2, dict->word[code][dict->word_len[code] - 1], 1);
	}

}

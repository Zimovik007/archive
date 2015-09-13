#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extract_lzw.h"
#include "compress_lzw.h"

int current_code_len;

inline static int is_two_degree(int a)
{
	return !(a&(a-1));
}

static code_t read_code(FILE *archf)
{
	static int  k = 7;
	static char c;
	code_t code = 0;
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

static string_t * print_word(FILE *orig, dictionary_t *dict, code_t code)
{
	string_t *str = get_word(dict, code);
	fwrite(str->chars, str->length, sizeof(byte_t), orig);
	return str;
}

extern void extract_lzw(FILE *archf, filesize_t origsize, FILE *orig)
{
	current_code_len = 8;
	dictionary_t *dict = create_dict(EXTRACT_MODE);
	string_t     *str  = create_str();
	string_t     *read_word = NULL;
	code_t code = 0;
	for(int i = 0; i < CHARS_NUM; add_word(dict, c_assign(str, i++)));
	while(!feof(archf))
	{
		code = read_code(archf);
		printf("<<%d>>", code);
		read_word = print_word(orig, dict, code);
		str = s_append(str, read_word->chars[0]);
		add_word(dict, str);
		str = s_assign(str, read_word->chars, read_word->length);
		if (is_two_degree(dict->cardinality - 1)) ++current_code_len;
		free(read_word);
	}
}

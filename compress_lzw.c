#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compress_lzw.h"

int current_code_len;

inline static int is_two_degree(int a)
{
	return !(a&(a-1));
}

static void codify(FILE *orig, FILE *archf, filesize_t *orig_size, filesize_t *archf_size)
{

}

extern void compress_lzw(FILE *orig, FILE *archf, filesize_t *orig_size, filesize_t *archf_size)
{
	rewind(orig);
	*orig_size = *archf_size = 0;
	current_code_len = 8;
	dictionary_t *dict = create_dict();
	string_t     *str  = create_str();
	for(int i = 0; i < CHARS_NUM; add_word(dict, assign(str, i++)));
	int last_existing_code, k = 7, code, bit;
	int does_word_exist; //-1 if doesn't exist
	byte_t c, prin_c = 0;
	while (1)
	{
		if (fread(&c, sizeof(byte_t), 1, orig) == 0) break;
		append(str, c);
		does_word_exist = get_code(dict, str);
		if (does_word_exist == -1)
		{
			code = last_existing_code;
			for(int i = current_code_len - 1; i >= 0; i--)
			{
				bit = !!(code & (1 << i));
				prin_c = prin_c | (bit << k);
				if (--k == -1)
				{
					fwrite(&prin_c, sizeof(byte_t), 1, archf);
					prin_c = 0;
					k = 7;
					++*archf_size;
				}
			}
			add_word(dict, str);
			assign(str, c);
			for(byte_t i = 0; i != c; last_existing_code = ++i);
			if (is_two_degree(dict->cardinality - 1)) ++current_code_len;
		}
		else
			last_existing_code = does_word_exist;
	}
	if (k < 7)
	{
		fwrite(&prin_c, sizeof(byte_t), 1, archf);
		++*archf_size;
	}
}

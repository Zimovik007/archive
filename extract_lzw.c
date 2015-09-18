#include "extract_lzw.h"

int current_code_len;

inline static int is_two_degree(int a)
{
	return !(a&(a-1));
}

static code_t read_code(FILE *archf)
{
	static int k = 7;
	static byte_t c;
	code_t code = 0;
	if (k == 7)
		if (fread(&c, sizeof(byte_t), 1, archf) == 0) return code;
	for(int i = current_code_len - 1; i >= 0; i--)
	{
		int bit = !!(c & (1 << k));
		code = code | (bit << i);
		if (--k == -1)
		{
			k = 7;
			if (i != 0)
				if (fread(&c, sizeof(byte_t), 1, archf) == 0) return code;
		}
	}
	return code;
}

static string_t * print_word(FILE *orig, dictionary_t *dict, code_t code, code_t prev_code)
{
	string_t *str = get_word(dict, code);
	if (str == NO_WORD)
	{
		string_t *prev_str = get_word(dict, prev_code);
		str = create_str();
		str = s_assign(str, prev_str->chars, prev_str->length);
		str = s_append(str, prev_str->chars[0]);
		add_word(dict, str);
		free(str);
		str = get_word(dict, code);
	}
	fwrite(str->chars, str->length, sizeof(byte_t), orig);
	return str;
}

extern void extract_lzw(FILE *archf, filesize_t origsize, FILE *orig)
{
	current_code_len = 8;
	dictionary_t *dict = create_dict(EXTRACT_MODE);
	string_t     *str  = create_str();
	string_t     *read_word = NULL;
	filesize_t   printed_cnt = 0;
	code_t       code = 0, prev_code = 0;
	for(int i = 0; i < CHARS_NUM; add_word(dict, c_assign(str, i++)));
	free(str);
	str = create_str();
	while(!feof(archf))
	{
		code = read_code(archf);
		if (code == -1 || printed_cnt == origsize) break;
		read_word = print_word(orig, dict, code, prev_code);
		printed_cnt += read_word->length;
		str = s_append(str, read_word->chars[0]);
		if (get_code(dict, str) == -1) add_word(dict, str);
		str = s_assign(str, read_word->chars, read_word->length);
		if (is_two_degree(dict->cardinality)) ++current_code_len;
		prev_code = code;
	}
}

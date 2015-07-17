#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extract_lzw.h"
#include "compress_lzw.h"

int current_code_len;

static int take_code(FILE *archf)
{
	static int k = 7;
	static char c;
	int code = 0, i, bit;
	if (k == 7)
		if (fscanf(archf, "%c", &c) <= 0) return code;
	for(i = current_code_len - 1; i >= 0; i--)
	{
		bit = !!(c & (1 << k));
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
	memcpy(new_str->chars, s1->chars, s1->length * sizeof(char));
	memcpy(new_str->chars + s1->length, s2->chars, s2->length * sizeof(char));
	new_str->length = s1->length + s2->length;
	//free(s1); free(s2);
	return new_str;	
}

extern void extract_lzw(FILE *archf, FILE *orig, unsigned int orig_size)
{
	rewind(archf);
	current_code_len = 8;
	dictionary_t *dict = create_dict();
	string_t *s1 = create_str(), *s2 = create_str();
	int i;
	for(i = 0; i < CHARS_NUM; i++)
	{
		assign(s1, (char)i);
		add_to_dictionary(dict, s1);		
	}	
	
	int t, code;
	char c;
	fscanf(archf, "%c", &c); 
	assign(s1, c);
	while (!feof(archf))
	{
		//if (fscanf(archf, "%c", &c) <= 0) break;
		code = take_code(archf);
		if (code <= dict->size)
		{
			assign_s(s2, dict->word[code], dict->word_len[code]);
		}
		s1 = concat_str(s1, s2);
		printf("%s %d\n", s1->chars, s1->length);	
		t = dict_str_id(dict, s1);
		if (t == -1)
		{
			add_to_dictionary(dict, s1);
			assign(s1, s1->chars[s1->length - 1]);
		}
	}
	//printf("%d\n", dict->size);
	//for(i = 0; i < dict->size; i++)
		//printf("%s %d\n", dict->word[i], dict->word_len[i]);	
}

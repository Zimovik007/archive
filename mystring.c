#include "mystring.h"

extern string_t * create_str()
{
	string_t *new_str = (string_t*)malloc(sizeof(string_t));
	new_str->length = 0;
	new_str->chars  = NULL;
	return new_str;
}

inline extern void append(string_t *str, char c)
{
	int length = ++str->length;
	str->chars[length - 1] = c;
}

inline extern void assign(string_t *str, char c)
{
	str->length = 1;
	str->chars[0] = c;
}

#include "mystring.h"

extern string_t * create_str()
{
	string_t *new_str = (string_t*)malloc(sizeof(string_t));
	new_str->length = 0;
	new_str->chars  = NULL;
	return new_str;
}

inline extern string_t * append(string_t *str, byte_t c)
{
	int length = setlength(str, str->length + 1);
	str->chars[length - 1] = c;
	return str;
}

inline extern string_t * assign(string_t *str, byte_t c)
{
	setlength(str, 1);
	str->chars[0] = c;
	return str;
}

inline extern int setlength(string_t *str, const int length)
{
	str->length = length;
	str->chars  = (byte_t*)realloc(str->chars, length);
	return length;
}

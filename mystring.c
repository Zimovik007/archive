#include "mystring.h"

extern string_t * create_str()
{
	string_t *new_str = (string_t*)malloc(sizeof(string_t));
	new_str->length = 0;
	new_str->chars  = NULL;
	return new_str;
}

extern inline string_t * s_append(string_t *str, byte_t c)
{
	int length = s_setlength(str, str->length + 1);
	str->chars[length - 1] = c;
	return str;
}

extern inline string_t * c_assign(string_t *str, byte_t c)
{
	s_setlength(str, 1);
	str->chars[0] = c;
	return str;
}

extern inline int s_setlength(string_t *str, const int length)
{
	str->length = length;
	str->chars  = (byte_t*)realloc(str->chars, length);
	return length;
}

extern string_t * s_concat(string_t *s1, string_t *s2)
{
	string_t *new_str = create_str();
	s_setlength(new_str, s1->length + s2->length);
	memcpy(new_str->chars,              s1->chars, s1->length * sizeof(byte_t));
	memcpy(new_str->chars + s1->length, s2->chars, s2->length * sizeof(byte_t));
	return new_str;
}

extern string_t * s_assign(string_t *str, byte_t *chars, int length)
{
	s_setlength(str, length);
	memcpy(str->chars, chars, length * sizeof(byte_t));
	return str;
}

extern void s_print(string_t *str)
{
	for(int i = 0; i < str->length; i++)
		printf("%c", str->chars[i]);
}







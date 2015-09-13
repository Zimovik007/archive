#ifndef MYSTRING_H_
#define MYSTRING_H_

#include <stdlib.h>
#include <string.h>
#include "types.h"

typedef struct string_t
{
	byte_t *chars;
	int     length;
} string_t;

extern string_t * create_str();

extern inline string_t * s_append(string_t *, byte_t);

extern inline string_t * c_assign(string_t *, byte_t);

extern inline int s_setlength(string_t *, const int);

extern string_t * s_concat(string_t *, string_t *);

extern string_t * s_assign(string_t *, byte_t *, int);

#endif /* MYSTRING_H_ */

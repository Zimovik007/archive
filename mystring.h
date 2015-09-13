#ifndef MYSTRING_H_
#define MYSTRING_H_

#include <stdlib.h>
#include "types.h"

typedef struct string_t
{
	byte_t *chars;
	int     length;
} string_t;

extern string_t * create_str();

inline extern string_t * append(string_t *str, byte_t c);

inline extern string_t * assign(string_t *str, byte_t c);

inline extern int setlength(string_t *str, int length);

#endif /* MYSTRING_H_ */

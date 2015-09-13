#ifndef MYSTRING_H_
#define MYSTRING_H_

#include <stdlib.h>

typedef struct string_t
{
	char *chars;
	int   length;
} string_t;

extern string_t * create_str();

inline extern void append(string_t *str, char c);

inline extern void assign(string_t *str, char c);


#endif /* MYSTRING_H_ */

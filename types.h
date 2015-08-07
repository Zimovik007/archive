#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#define CHARS_NUM (256)
#define ASCII_0 (48)

typedef
	unsigned long long filesize_t;

typedef
    unsigned char byte_t;

typedef
	struct string_t
	{
		char chars[1000];
		int length;
	} string_t;

#endif // TYPES_H_INCLUDED

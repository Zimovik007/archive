#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#define CHARS_NUM (256)
#define ASCII_0 (48)
#define METHODS_NUM (4)

typedef unsigned long long filesize_t;

typedef void (*extract_method_t)(FILE *, filesize_t, FILE *);

typedef void (*compress_method_t)(FILE *orig, FILE *archf, filesize_t *original_size, filesize_t *compressed_size);

typedef unsigned char byte_t;

typedef struct string_t
	{
		char chars[1000];
		int length;
	} string_t;

typedef enum
	{
		HUFFMAN_ALG, NO_COMPRESS, LZW_ALG, ERR_ALG
	} _algorithm_t;

typedef int algorithm_t;

typedef enum
	{
		COMPRESS_MODE, EXTRACT_MODE
	} _arch_mode;

typedef int arch_mode;

#endif // TYPES_H_INCLUDED

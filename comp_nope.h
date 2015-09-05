#ifndef COMPRESS_NOPE
#define COMPRESS_NOPE

#include <stdio.h>
#include "types.h"

extern void compress_nope(FILE *orig, FILE *archf, filesize_t *original_size, filesize_t *compressed_size);

#endif

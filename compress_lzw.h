#ifndef COMPRESS_LZW_H
#define COMPRESS_LZW_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "dict.h"
#include "mystring.h"

extern void compress_lzw(FILE *, FILE *, filesize_t *, filesize_t *);

#endif

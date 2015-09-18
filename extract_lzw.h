#ifndef EXTRACT_LZW_H
#define EXTRACT_LZW_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "mystring.h"
#include "dict.h"

extern void extract_lzw(FILE *, filesize_t, FILE *);

#endif

#ifndef BINARY_RW_H
#define BINARY_RW_H

#include <stdio.h>
#include <stdlib.h>
#include "types.h"

extern void print_bin_header(FILE *fout, int files_count, int is_solid);

extern void print_bin_fat_entry(FILE *fout, int fname_len, char *fname, filesize_t packed_size, filesize_t orig_size, int is_solid);

extern int f_is_upa(FILE *fin);

extern int f_is_solid(FILE *fin);

extern int f_fname_len(FILE *fin);

extern char* f_fname(FILE *fin, int fname_len);

extern filesize_t f_int_read(FILE *fin, int size);

extern void f_read_attributes(FILE *fin);

#endif

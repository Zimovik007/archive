#include "binary_rw.h"

char * FORMAT_DESCR = "UPA";

char * HUFF_DESCR = "HUFF";
char * LZW_DESCR  = "LZWA";
char * NOPE_DESCR = "NOPE";

extern void print_bin_header(FILE *fout, int files_count, int is_solid, algorithm_t algo)
{
	fwrite(FORMAT_DESCR, sizeof(char), strlen(FORMAT_DESCR), fout);
	char * algo_descr;
	switch (algo)
	{
		case HUFFMAN_ALG: algo_descr = HUFF_DESCR; break;
		case NO_COMPRESS: algo_descr = NOPE_DESCR; break;
		case LZW_ALG: algo_descr = LZW_DESCR; break;
		default: algo_descr = NULL;
	}
	fwrite(algo_descr, sizeof(char), strlen(algo_descr), fout);
	char iss = (char)is_solid;
	fwrite(&iss, 1, 1, fout);
	unsigned short int f_count = (unsigned short int)files_count;
	fwrite(&f_count, 2, 1, fout);
}

extern filesize_t print_bin_fat_entry(FILE *fout, int fname_len, char *fname, filesize_t packed_size, filesize_t orig_size, int is_solid)
{
	char fileattr = 0;
	int filetime = -1;
	--fname_len;
	byte_t fl = (byte_t)fname_len;
	fwrite(&fl, 1, 1, fout);
	//print filename
	fwrite(fname, 1, fname_len + 1, fout);
	filesize_t print_size_shift = ftell(fout);
	f_num_write(fout, packed_size, sizeof(filesize_t));
	f_num_write(fout, orig_size, sizeof(filesize_t));
	fwrite(&fileattr, 1, 1, fout);
	fwrite(&filetime, sizeof(int), 1, fout);
	return print_size_shift;
}

extern void f_num_write(FILE *fout, filesize_t num, size_t size)
{
	fwrite(&num, size, 1, fout);
}

extern algorithm_t f_algo(FILE *fin)
{
	const int CCOUNT = 4;
	char      *str   = (char*)malloc((CCOUNT + 1) * sizeof(char));
	str[4] = '\0';
	fread(str, sizeof(char), CCOUNT, fin);
	if (strcmp(str, HUFF_DESCR) == 0)
		return HUFFMAN_ALG;
	else
	if (strcmp(str, LZW_DESCR) == 0)
		return LZW_ALG;
	else
	if (strcmp(str, NOPE_DESCR) == 0)
		return NO_COMPRESS;
	else return ERR_ALG;
}

extern int f_is_upa(FILE *fin)
{
	char *c = calloc(4, sizeof(char));
	fread(c, sizeof(char), 3, fin);
	return strcmp(c, "UPA") == 0;
}

extern int f_is_solid(FILE *fin)
{
	char c;
	fread(&c, 1, 1, fin);
	return (int)c;
}

extern int f_fname_len(FILE *fin)
{
	byte_t c;
	fread(&c, 1, 1, fin);
	return (int)c + 1;
}

extern char* f_fname(FILE *fin, int fname_len)
{
	char *fname = (char*)malloc((fname_len + 1) * sizeof(char));
	fread(fname, sizeof(char), fname_len, fin);
	fname[fname_len] = '\0';
	return fname;
}

extern filesize_t f_int_read(FILE *fin, int size)
{
	filesize_t res = 0;
	fread(&res, size, 1, fin);
	return res;
}

extern void f_read_attributes(FILE *fin)
{
	fseek(fin, 2, SEEK_CUR);
}

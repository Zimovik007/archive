#include "binary_rw.h"

extern void print_bin_header(FILE *fout, int files_count, int is_solid)
{
	fprintf(fout, "UPAHUFF%c", (char)is_solid);
	int i, j;
	char bit;
	unsigned short int f_count = (unsigned short int)files_count;
	for(i = 1; i >= 0; i--)
	{
		char c = 0;
		for(j = 7; j >= 0; j--)
		{
			bit = !!(f_count & (1 << (i*8 + j)));
			c = c | (bit << j);
		}
		fprintf(fout, "%c", c);
	}
}

extern void print_bin_fat_entry(FILE *fout, int fname_len, char *fname, filesize_t packed_size, filesize_t orig_size, int is_solid)
{
	int i, j, bit;
	--fname_len;
	filesize_t p_size = packed_size;
	filesize_t o_size = orig_size;
	fprintf(fout, "%c", (unsigned char)fname_len);
	for(i = 0; i < fname_len + 1; i++)
		fprintf(fout, "%c", fname[i]);
	//packed size
	for(i = 7; i >= 0; i--){
		char c = 0;
		for(j = 7; j >= 0 && !is_solid; j--){
			bit = !!(p_size & (1 << (i*8 + j)));
			c = c | (bit << j);
		}
		fprintf(fout, "%c", c);
	}
	//orig_size
	for(i = 7; i >= 0; i--){
		char c = 0;
		for(j = 7; j >= 0; j--){
			bit = !!(o_size & (1 << (i*8 + j)));
			c = c | (bit << j);
		}
		fprintf(fout, "%c", c);
	}
	fprintf(fout, "%c%c%c%c%c", 0, 0, 0, 0, 0);
}


extern int f_is_upa(FILE *fin)
{
	char u, p, a;
	fscanf(fin, "%c%c%c", &u, &p, &a);
	return u == 'U' && p == 'P' && a == 'A';
}

extern int f_is_solid(FILE *fin)
{
	char c;
	fscanf(fin, "%c", &c);
	return (int)c;
}

extern int f_fname_len(FILE *fin)
{
	unsigned char c;
	fscanf(fin, "%c", &c);
	return (int)c + 1;
}

extern char* f_fname(FILE *fin, int fname_len)
{
	int i;
	char *fname = (char*)malloc((fname_len + 1) * sizeof(char));
	for(i = 0; i < fname_len; i++)
		fscanf(fin, "%c", &fname[i]);
	fname[fname_len] = '\0';
	return fname;
}

extern filesize_t f_int_read(FILE *fin, int size)
{
	filesize_t res = 0, bit;
	int i, j;
	char c;
	for(i = size - 1; i >= 0; i--)
	{
		fscanf(fin, "%c", &c);
		for(j = 7; j >= 0; j--)
		{
			bit = !!(c & (1 << j));
			res = res | (bit << (i*8 + j));
		}
	}
	return res;
}

extern void f_read_attributes(FILE *fin)
{
	char c;
	fscanf(fin, "%c%c%c%c%c", &c, &c, &c, &c, &c);
}

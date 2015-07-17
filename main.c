#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "compress.h"
#include "extract_huff.h"
#include "extract_nope.h"
#include "comp_nope.h"
#include "compress_lzw.h"
#include "extract_lzw.h"

typedef
	unsigned long long filesize_t;
	

void print_bin_header(FILE *fout, int files_count, int is_solid)
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

void print_bin_fat_entry(FILE *fout, int fname_len, char *fname, filesize_t packed_size, filesize_t orig_size, int is_solid)
{
	int i, j, bit;
	--fname_len;
	filesize_t p_size = packed_size;
	filesize_t o_size = orig_size;
	fprintf(fout, "%c", (unsigned char)fname_len);
	for(i = 0; i < fname_len; i++)
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


int f_is_upa(FILE *fin)
{
	char u, p, a;
	fscanf(fin, "%c%c%c", &u, &p, &a);
	return u == 'U' && p == 'P' && a == 'A';
}

int f_is_solid(FILE *fin)
{
	char c;
	fscanf(fin, "%c", &c);
	return (int)c;
}

int f_fname_len(FILE *fin)
{
	unsigned char c;
	fscanf(fin, "%c", &c);
	return (int)c;
}

char* f_fname(FILE *fin, int fname_len)
{
	int i;
	char *fname = (char*)malloc((fname_len + 1) * sizeof(char));
	for(i = 0; i < fname_len; i++)
		fscanf(fin, "%c", &fname[i]);
	fname[fname_len] = '\0';
	return fname;
}

filesize_t f_int_read(FILE *fin, int size)
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

void f_read_attributes(FILE *fin)
{
	char c;
	fscanf(fin, "%c%c%c%c%c", &c, &c, &c, &c, &c);
}

void add_to_list(char **list, char *filename, int filescount)
{
	list = (char**)realloc(list, filescount * sizeof(char*));
	list[filescount - 1] = filename;
}

void compress(char **files, int *file_exists, int filescount)
{
	
}

void extract(char **files, int *file_exists, int filescount)
{
	
	
}

int identify_action(char **files, int *file_exists, int filescount)
{
	for(int i = 0; i < filescount; i++)
	{
		
		
	}
	return 1;
}

int main(int argc, char* argv[])
{
	int *keys = calloc(CHARS_NUM, sizeof(int)), filescount = 0;
	char **files = NULL;
	for(int i = 1; i < argc; i++)
	{
		if (strlen(argv[i]) >= 2 && argv[i][0] == '-')
			keys[(int)argv[i][1]] = 1;
		else
			add_to_list(files, argv[i], ++filescount);
	}

	int *file_exists = malloc(filescount * sizeof(int));
	for(int i = 1; i < filescount; i++)
	{
		FILE *file = fopen(files[i], "rb");
		file_exists[i] = !!file;
		if (file) fclose(file);
	}
	
	if (keys[(int)'d'] && !keys[(int)'e'])
		compress(files, file_exists, filescount);
	else
	if (!keys[(int)'d'] && keys[(int)'e'])
		extract(files, file_exists, filescount);
	else
	{
		if (identify_action(files, file_exists, filescount))
			compress(files, file_exists, filescount);
		else
			extract(files, file_exists, filescount);
	}		
	
	return 0;
}

/*
void archive(char* argv[], int argc)
{
	time_t t = time(NULL);
	struct tm* ctme = localtime(&t);
	printf("%02d:%02d:%02d compressing...\n", ctme->tm_hour, ctme->tm_min, ctme->tm_sec);
}
*/

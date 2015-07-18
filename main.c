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

#define COMPR (keys[(int)'c'])
#define DEL_IN (keys[(int)'d'])
#define EXTR (keys[(int)'e'])
#define NO_INF (keys[(int)'n'])
#define SOLID (keys[(int)'s'])
#define SH_TIME (keys[(int)'t'])
/*
 * COMPR = compress
 * DEL_IN = delete input files
 * EXTR = extract
 * NO_INF = no information
 * SOLID = solid
 * SH_TIME = show time
 */
	
int keys[CHARS_NUM];
 
struct tm * gettime()
{
	time_t t = time(NULL);
	struct tm *ctme = localtime(&t);
	return ctme;
}

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

char ** add_to_list(char **files, char *filename, int *filescount)
{
	if (strlen(filename) == 1 && (filename[0] == '\\' || filename[0] == '>')) return files;
	++*filescount;
	files = (char**)realloc(files, *filescount * sizeof(char*));
	files[*filescount - 1] = filename;
	return files;
}

void read_block(char *buffer, FILE *stream)
{
	

	return;
}

void print_time(char *str_before, char *str_after)
{
	if (!SH_TIME || NO_INF) return;
	struct tm *t;
	t = gettime();
	printf("%s%02d:%02d:%02d%s", str_before, t->tm_hour, t->tm_min, t->tm_sec, str_after);
}

void compress_nosolid(FILE *archf, char **files, int *file_exists, int filescount, int newfile_id)
{
	const int BLOCK_LEN = 4096;
	for(int i = 0; i < filescount; i++)
	{
		if (!file_exists[i] || i == newfile_id) continue;
		FILE *orig = fopen(files[i], "rb");
		print_time("", " ");
		if (!NO_INF) printf("  %s", files[i]);
		fflush(stdout);
		filesize_t orig_size, compressed_size;
		FILE *filebuf = compress_huffman(orig, &orig_size, &compressed_size);
		void *buf = malloc(BLOCK_LEN);
		rewind(filebuf);
		print_bin_fat_entry(archf, strlen(files[i]) + 1, files[i], orig_size, compressed_size, 0);
		int left_to_write = compressed_size;
		while (left_to_write > 0)
		{
			fread(buf, 1, left_to_write >= BLOCK_LEN ? BLOCK_LEN : left_to_write, filebuf);
			fwrite(buf, 1, left_to_write >= BLOCK_LEN ? BLOCK_LEN : left_to_write, archf);
			left_to_write -= BLOCK_LEN;
		}
		fclose(orig);
		if (!NO_INF) printf(" +\n");
	}	
}

void compress(char **files, int *file_exists, int filescount)
{
	FILE *archf = NULL;
	print_time("", " ");
	if (!NO_INF) printf(" compressing files:\n");
	int exist_count = 0, newfile_id = -1;
	for(int i = filescount - 1; i >= 0; i--)
	{
		if (files[i][0] == '>' && newfile_id == -1)
			newfile_id = i;
		if (files[i][0] == '\\' || files[i][0] == '>')
			files[i] = files[i] + 1;
	}
	if (newfile_id == -1)
		for(int i = filescount - 1; i >= 0 && file_exists[i]; newfile_id = --i);
	for(int i = 0; i < filescount; exist_count += !!file_exists[i] && i++ != newfile_id);
	if (newfile_id == -1)
	{
		struct tm *t = gettime();
		char curtime[40];
		strftime(curtime, 40, "arch_%Y-%m-%d_%X.upa", t);
		archf = fopen(curtime, "wb");
	}
	else
		archf = fopen(files[newfile_id], "wb");
		
	print_bin_header(archf, exist_count, 0);
	compress_nosolid(archf, files, file_exists, filescount, newfile_id);
	fclose(archf);
	print_time("", " ");
	if (!NO_INF) printf(" compete\n");
}

void extract(char **files, int *file_exists, int filescount)
{
	//printf("extract\n");
}

int identify_action(char **files, int *file_exists, int filescount)
{
	const int COMPRESS = 1, EXTRACT = 0;
	for(int i = 0; i < filescount; i++)
	{
		if (!file_exists[i]) continue;
		FILE *file = fopen(files[i], "rb");
		if (!f_is_upa(file))
		{
			fclose(file);
			return COMPRESS;
		}
		fclose(file);
	}
	return EXTRACT;
}

int main(int argc, char* argv[])
{
	int filescount = 0;
	char **files = NULL;
	for(int i = 1; i < argc; i++)
	{
		if (strlen(argv[i]) >= 2 && argv[i][0] == '-')
			keys[(int)argv[i][1]] = 1;
		else
		{
			files = add_to_list(files, argv[i], &filescount);
		}
	}
	
	int *file_exists = malloc(filescount * sizeof(int));
	for(int i = 0; i < filescount; i++)
	{
		FILE *file = fopen(files[i], "rb");
		file_exists[i] = !!file;
		if (file) fclose(file);
	}
	
	if (COMPR && !EXTR)
		compress(files, file_exists, filescount);
	else
	if (!COMPR && EXTR)
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

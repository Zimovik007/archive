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
#include "types.h"
#include "binary_rw.h"

#define COMPR     (keys[(int)'c'])
#define DEL_INPUT (keys[(int)'d'])
#define EXTR      (keys[(int)'e'])
#define NO_INF    (keys[(int)'n'])
#define SOLID     (keys[(int)'s'])
#define SHOW_TIME (keys[(int)'t'])
#define SAVE_SYMB ('=')
/*
 * COMPR = compress
 * DEL_IN = delete input files
 * EXTR = extract
 * NO_INF = no information
 * SOLID = solid
 * SH_TIME = show time
 */
#define COMPRESS_MODE (1)
#define EXTRACT_MODE  (0)

int keys[CHARS_NUM];

struct tm * gettime()
{
	time_t     t    = time(NULL);
	struct tm *ctme = localtime(&t);
	return ctme;
}

char ** add_to_list(char **files, char *filename, int *filescount)
{
	if (strlen(filename) == 1 && filename[0] == SAVE_SYMB) return files;
	++*filescount;
	files = (char**)realloc(files, *filescount * sizeof(char*));
	files[*filescount - 1] = filename;
	return files;
}

void print_time(char *str_before, char *str_after)
{
	if (!SHOW_TIME || NO_INF) return;
	struct tm *t = gettime();
	printf("%s%02d:%02d:%02d%s", str_before, t->tm_hour, t->tm_min, t->tm_sec, str_after);
}

void compress_nosolid(FILE *archf, char **files, int *file_exists, int filescount, int newfile_id)
{
	const int BLOCK_LEN = 4096;

	for(int i = 0; i < filescount; i++)
	{
		if (!file_exists[i] || i == newfile_id) continue;
		FILE      *orig = fopen(files[i], "rb");
		void      *buf  = malloc(BLOCK_LEN);
		filesize_t orig_size;
		filesize_t compressed_size;

		print_time("", " ");
		if (!NO_INF) printf("  %s", files[i]);
		fflush(stdout);

		FILE *filebuf = compress_huffman(orig, &orig_size, &compressed_size);
		rewind(filebuf);

		print_bin_fat_entry(archf, strlen(files[i]) + 1, files[i], orig_size, compressed_size, 0);
		int left_to_write = compressed_size;
		while (left_to_write > 0)
		{
			fread (buf, 1, left_to_write >= BLOCK_LEN ? BLOCK_LEN : left_to_write, filebuf);
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
	char *archname;
	int   exist_count =  0;
	int   newfile_id  = -1;

	print_time("", " ");
	if (!NO_INF) printf(" compressing files:\n");

	for(int i = filescount - 1; i >= 0; i--)
		if (files[i][0] == SAVE_SYMB && newfile_id == -1)
		{
			newfile_id = i;
			files[i] = files[i] + 1;
			break;
		}

	if (newfile_id == -1)
		for(int i = filescount - 1; i >= 0 && file_exists[i]; newfile_id = --i);

	for(int i = 0; i < filescount; i++)
		exist_count += !!file_exists[i] && i != newfile_id;

	if (newfile_id == -1)
	{
		struct tm *t = gettime();
		archname = (char*)calloc(50, sizeof(char));
		strftime(archname, 50, "arch_%Y-%m-%d_%X.upa", t);
	}
	else
		archname = files[newfile_id];

	archf = fopen(archname, "wb");
	print_bin_header(archf, exist_count, 0);

	if (!SOLID)
		compress_nosolid(archf, files, file_exists, filescount, newfile_id);
	fclose(archf);
	print_time("", " ");

	if (!NO_INF) printf(" compete -> %s\n", archname);
}

void extract(char **files, int *file_exists, int filescount)
{
	printf("extract\n");
}

int identify_action(char **files, int *file_exists, int filescount)
{
	for(int i = 0; i < filescount; i++)
	{
		if (!file_exists[i]) continue;
		FILE *file = fopen(files[i], "rb");
		if (!f_is_upa(file))
		{
			fclose(file);
			return COMPRESS_MODE;
		}
		fclose(file);
	}
	return EXTRACT_MODE;
}

int main(int argc, char* argv[])
{
	int    filescount = 0;
	char **files      = NULL;

	for(int i = 1; i < argc; i++)
	{
		if (strlen(argv[i]) >= 2 && argv[i][0] == '-')
			keys[(int)argv[i][1]] = 1;
		else
			files = add_to_list(files, argv[i], &filescount);
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
		if (identify_action(files, file_exists, filescount) == COMPRESS_MODE)
			compress(files, file_exists, filescount);
		else
			extract(files, file_exists, filescount);
	}
	return 0;
}

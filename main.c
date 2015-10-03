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
#include "command_line_args.h"

int     keys[CHARS_NUM];
#define COMPR     (keys[(int)'c']) //compress
#define DEL_INPUT (keys[(int)'d']) //delete input files
#define EXTR      (keys[(int)'e']) //extract
#define NO_INF    (keys[(int)'n']) //no information
#define SOLID     (keys[(int)'s']) //archive is solid
#define SHOW_TIME (keys[(int)'t']) //show time
#define SAVE_SYMB ('=')

int     algs[METHODS_NUM + 1];
#define AL_HUFF (algs[HUFFMAN_ALG])
#define AL_LZW  (algs[LZW_ALG])
#define AL_NOPE (algs[NO_COMPRESS])

cl_check_t *checkres;

compress_method_t get_comp_func(algorithm_t method);
extract_method_t  get_extr_func(algorithm_t method);

struct tm * gettime()
{
	time_t t = time(NULL);
	return localtime(&t);
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

compress_method_t get_comp_func(algorithm_t method)
{
	if (method == HUFFMAN_ALG) return compress_huffman;
	if (method == LZW_ALG)     return compress_lzw;
	if (method == NO_COMPRESS) return compress_nope;
	return NULL;
}

void compress_nosolid(FILE *archf, char **files, int *file_exists, int filescount, int newfile_id, algorithm_t method)
{
	compress_method_t start_compressing = get_comp_func(method);
	for(int i = 0; i < filescount; i++)
	{
		if (!file_exists[i] || i == newfile_id) continue;
		FILE      *orig = fopen(files[i], "rb");
		filesize_t orig_size;
		filesize_t compressed_size;

		print_time("", " ");
		if (!NO_INF) printf("  %s", files[i]);
		fflush(stdout);
		filesize_t print_size_shift = print_bin_fat_entry(archf, strlen(files[i]), files[i], compressed_size, orig_size, 0);

		start_compressing(orig, archf, &orig_size, &compressed_size);

		fseek(archf, print_size_shift, SEEK_SET);
		f_num_write(archf, compressed_size, sizeof(filesize_t));
		f_num_write(archf, orig_size, sizeof(filesize_t));
		fseek(archf, 0, SEEK_END);

		fclose(orig);
		if (DEL_INPUT) remove(files[i]);
		if (!NO_INF) printf(" +\n");
	}
}

void compress(char **files, int *file_exists, int filescount)
{
	FILE *archf       = NULL;
	char *archname    = NULL;
	int   exist_count = 0;
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

	algorithm_t i;
	for(i = 0; i < METHODS_NUM + 1 && !algs[i]; i++);
	algorithm_t method = i < METHODS_NUM ? i : HUFFMAN_ALG;

	archf = fopen(archname, "wb");
	print_bin_header(archf, exist_count, 0, method);

	if (!SOLID)
		compress_nosolid(archf, files, file_exists, filescount, newfile_id, method);

	fclose(archf);
	print_time("", " ");

	if (!NO_INF) printf(" %s << complete\n", archname);
}

extract_method_t get_extr_func(algorithm_t method)
{
	if (method == HUFFMAN_ALG) return extract_huffman;
	if (method == LZW_ALG)     return extract_lzw;
	if (method == NO_COMPRESS) return extract_nope;
	return NULL;
}

void process_archfile(char **files, int *file_exists, int filescount)
{
	for(int archfcnt = 0; archfcnt < filescount; archfcnt++)
	{
		FILE *file = fopen(files[archfcnt], "rb");
		if (!file || !f_is_upa(file)) continue;
		algorithm_t      method           = f_algo(file);
		extract_method_t start_extracting = get_extr_func(method);
		if (!start_extracting)
		{
			fclose(file); continue;
		}
		int is_solid = f_is_solid(file);
		if (is_solid)
			continue;
		unsigned short int f_in_arch_amt = f_int_read(file, sizeof(unsigned short int));
		for(int fcnt = 0; fcnt < f_in_arch_amt; fcnt++)
		{
			int   fn_len    = f_fname_len(file);
			char *filename  = f_fname(file, fn_len);
			int   packsize  = f_int_read(file, sizeof(filesize_t));
			int   origsize  = f_int_read(file, sizeof(filesize_t));
			if (cl_is_true(AT_FILELIST))
			{
				char measure = origsize < 1024 ? 'B' : (origsize < 1024*1024 ? 'K' : 'M');
				double printsize = origsize < 1024 ? origsize : (origsize < 1024*1024 ? origsize/1024.0 : origsize/1024.0/1024.0);
				printf("  %d. %s (%.1f %c)\n", fcnt + 1, filename, printsize, measure);
				fseek(file, packsize, SEEK_CUR);
				continue;
			}
			if (cl_is_true(AT_GETFILE))
			{
				int get_it = 0;
				cl_check_t *check = cl_get_arg_res(AT_GETFILE);
				cl_split_nums(check);
				for(int i = 0; i < check->valcnt; i++)
					get_it = get_it || (strtol(check->values[i], (char**)NULL, 10) == fcnt + 1);
				if (!get_it)
				{
					fseek(file, packsize, SEEK_CUR);
					continue;
				}
			}
			FILE *origfile = fopen(filename, "wb");
			start_extracting(file, origsize, origfile);
			fclose(origfile);
		}
		fclose(file);
		if (DEL_INPUT) remove(files[archfcnt]);
	}
}

arch_mode identify_action(char **files, int *file_exists, int filescount)
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
	cl_init();
	for(int i = 1; i < argc; i++)
		cl_add_arg(argv[i]);
	int    filescount = 0;
	char **files      = NULL;
	for(int i = 1; i < argc; i++)
	{
		if (strlen(argv[i]) >= 2 && argv[i][0] == '-')
		{
			char key = argv[i][1];
			keys[(int)key] = 1;
			if (key == 'a' && strlen(argv[i]) > 3 && argv[i][2] == '=')
			{
				if (strcmp(argv[i] + 3, "huff") == 0) algs[HUFFMAN_ALG] = 1;
				if (strcmp(argv[i] + 3, "lzw")  == 0) algs[LZW_ALG]     = 1;
				if (strcmp(argv[i] + 3, "nope") == 0) algs[NO_COMPRESS] = 1;
			}
		}
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
		process_archfile(files, file_exists, filescount);
	else
	{
	if (identify_action(files, file_exists, filescount) == COMPRESS_MODE)
		compress(files, file_exists, filescount);
	else
		process_archfile(files, file_exists, filescount);
	}

	return 0;
}

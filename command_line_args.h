#ifndef COMMAND_LINE_ARGS_H_
#define COMMAND_LINE_ARGS_H_

#include "types.h"
#include "dict.h"

typedef enum
{
	AT_NONE, AT_COMPRESS, AT_DELINPUT, AT_EXTRACT, AT_NOINF, AT_SHOWTIME, AT_HUFFMAN, AT_LZW, AT_NOCOMPRESS, AT_FILELIST, AT_GETFILE, AT_COMPRESSFILE
} cl_argument_t;

typedef enum
{
	CLCDISABLED, CLC_ENABLED, CLC_NOOPTION
} cl_checkword_t;

typedef struct cl_check_word_result_t_
{
	int true;
	int valcnt;
	char **values;
} cl_check_t;

extern void add_arg(char *argument);

extern int get_arg(cl_argument_t arg);

#endif /* COMMAND_LINE_ARGS_H_ */

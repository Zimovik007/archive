#ifndef COMMAND_LINE_ARGS_H_
#define COMMAND_LINE_ARGS_H_

#include "types.h"
#include "dict.h"

typedef enum
{
	AT_NONE, AT_COMPRESS, AT_DELINPUT, AT_EXTRACT, AT_NOINF, AT_SHOWTIME, AT_HUFFMAN, AT_LZW, AT_NOCOMPRESS, AT_FILELIST, AT_GETFILE, AT_COMPRESSFILE, ARGCOUNT
} cl_argument_t;

typedef enum
{
	CLCDISABLED, CLC_ENABLED, CLC_NOOPTION
} cl_checkword_t;

typedef enum
{
	CLN_NONE, CLN_ROOT, CLN_GETWORDPARENT, CLN_WORDEND
} cl_nodetype_t;

typedef struct cl_node_t
{
	int childsamt;
	int enabled;
	char symb;
	cl_argument_t argtype;
	cl_nodetype_t nodetype;
	struct cl_node_t **childs;
} cl_node_t;

typedef struct cl_check_word_result_t_
{
	int true;
	int valcnt;
	char **values;
} cl_check_t;

extern cl_node_t * cl_init();

extern void cl_add_arg(char *argument);

extern cl_check_t * cl_get_arg_res(cl_argument_t arg);

extern int cl_is_true(cl_argument_t arg);

extern void cl_split_nums(cl_check_t *check);

#endif /* COMMAND_LINE_ARGS_H_ */

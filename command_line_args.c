#include "command_line_args.h"

int init = 0;

#define COMPRESS_STR     ("-c")
#define DELINPUT_STR     ("-del")
#define EXTRACT_STR      ("-e")
#define NOINF_STR        ("-noinf")
#define SHOWTIME_STR     ("-time")
#define HUFFMAN_STR      ("--huff")
#define LZW_STR          ("--lzw")
#define NOCOMPRESS_STR   ("--nope")
#define FILELIST_STR     ("-ls")
#define GETFILE_STR      ("-f=")
#define COMPRESSFILE_STR ("=")

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

cl_node_t *cl_dict_root = NULL;

static cl_node_t * cl_create_node(char c, cl_nodetype_t nodetype, cl_argument_t argtype)
{
	cl_node_t *new_node = (cl_node_t*)malloc(sizeof(cl_node_t));
	new_node->childsamt = 0;
	new_node->enabled   = argtype == CLN_NONE;
	new_node->symb      = c;
	new_node->argtype   = argtype;
	new_node->nodetype  = nodetype;
	new_node->childs    = NULL;
	return new_node;
}

static cl_node_t * cl_create_dict()
{
	return create_node((char)0, CLN_ROOT, CLN_ROOT);
}

static trie_node_t * cl_add_node(cl_node_t *parent, char c, cl_nodetype_t got, cl_nodetype_t reg)
{
	parent->childs = (cl_node_t**)realloc(parent->childs, ++parent->childsamt * sizeof(cl_node_t*));
	parent->childs[parent->childsamt - 1] = cl_create_node(c, got, reg);
	return parent->childs[parent->childsamt - 1];
}

static inline cl_node_t * cl_converted(cl_node_t *node, cl_nodetype_t nodetype, cl_argument_t argtype)
{
	node->nodetype = node->nodetype == CLN_NONE ? nodetype : node->nodetype;
	node->argtype = node->argtype == AT_NONE ? argtype : node->argtype;
	node->enabled = argtype == AT_NONE;
	return node;
}

static inline cl_node_t * cl_break_trought(cl_node_t *parent, char c, cl_nodetype_t nodetype, cl_argument_t argtype)
{
	int i;
	for(i = 0; i < parent->childsamt && parent->childs[i]->symb != c; i++);
	return i < parent->childsamt && parent->childs[i]->symb == c ?
		cl_converted(parent->childs[i], nodetype, argtype) : cl_add_node(parent, c, nodetype, argtype);
}

static inline cl_node_t * cl_trie_step(cl_node_t *parent, char c)
{
	if (!parent) return NULL;
	int i;
	for(i = 0; i < parent->childsamt && parent->childs[i]->symb != c; i++);
	return i < parent->childsamt && parent->childs[i]->symb == c ? parent->childs[i] : NULL;
}

static void cl_add_word(cl_node_t *parent, cl_argument_t argt, char *str)
{
	cl_node_t *cur_node = parent;
	for(int i = 0; i < strlen(str); i++)
		cur_node = cl_break_trought(cur_node, str[i], i == strlen(str) - 1 ? argt : CLN_NONE, AT_NONE);
}

static void cl_reg(cl_node_t *parent, cl_argument_t argt, char *argv)
{
	cl_node_t *cur_node = parent;
	for(int i = 0; i < strlen(argv); i++)
		cur_node = cl_break_trought(cur_node, argv[i], CLN_NONE, i == strlen(argv) - 1 ? argt : AT_NONE);
}

static cl_node_t * cl_get_word_node(cl_node_t *parent, char *argv)
{
	cl_node_t *cur_node = parent;
	for(int i = 0; i < strlen(argv); i++)
		cur_node = cl_trie_step(cur_node, argv[i]);
	return cur_node;
}

int get_childs_str_iter;
static void cl_recursive_get_childs(cl_node_t *parent, cl_check_t *check)
{
	if (!parent) return;
	check->values[check->valcnt - 1] = (char*)realloc(check->values, (get_childs_str_iter + 1) * sizeof(char));
	check->values[check->valcnt - 1][get_childs_str_iter - 1] = parent->symb;
	check->values[check->valcnt - 1][get_childs_str_iter] = '\0';
	if (parent->nodetype == CLN_WORDEND)
	{
		++check->valcnt;
		check->values = (char**)realloc(check->values, check->valcnt * sizeof(char*));
		check->values[check->valcnt - 1] =
			memcpy(check->values[check->valcnt - 2],
			       check->values[check->valcnt - 1],
			       strlen(check->values[check->valcnt - 2]) + 1);
	}
	for(int i = 0; i < parent->childsamt; i++)
	{
		++get_childs_str_iter;
		cl_recursive_get_childs(parent->childs[i], check);
		--get_childs_str_iter;
	}
}

static cl_check_t * cl_get_childs_list(cl_node_t *parent)
{
	cl_check_t *result = (cl_check_t*)malloc(sizeof(cl_check_t));
	result->true   = 0;
	result->valcnt = 0;
	result->values = NULL;
	if (!parent) return result;
	get_childs_str_iter = 0;
	result->valcnt = 1;
	result->values = (char**)malloc(sizeof(char*));
	cl_recursive_get_childs(parent, result);
	--result->valcnt;
	return result;
}

extern cl_node_t * cl_init()
{
	cl_dict_root = cl_create_dict();
	cl_reg(cl_dict_root, AT_COMPRESS    , COMPRESS_STR);
	cl_reg(cl_dict_root, AT_DELINPUT    , DELINPUT_STR);
	cl_reg(cl_dict_root, AT_EXTRACT     , EXTRACT_STR);
	cl_reg(cl_dict_root, AT_NOINF       , NOINF_STR);
	cl_reg(cl_dict_root, AT_SHOWTIME    , SHOWTIME_STR);
	cl_reg(cl_dict_root, AT_HUFFMAN     , HUFFMAN_STR);
	cl_reg(cl_dict_root, AT_LZW         , LZW_STR);
	cl_reg(cl_dict_root, AT_NOCOMPRESS  , NOCOMPRESS_STR);
	cl_reg(cl_dict_root, AT_FILELIST    , FILELIST_STR);
	cl_reg(cl_dict_root, AT_GETFILE     , GETFILE_STR);
	cl_reg(cl_dict_root, AT_COMPRESSFILE, COMPRESSFILE_STR);
	init = 1;
	return cl_dict_root;
}

extern void cl_add_arg(char *argument)
{
	if (!init) return;
	cl_add_word(cl_dict_root, argument, CLN_WORDEND);
}



extern cl_check_t get_arg(cl_argument_t arg)
{
	cl_check_t result;
	result.true   = 0;
	result.valcnt = 0;
	result.values = NULL;
	if (!init) return result;


	return result;
}

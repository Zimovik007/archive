#include "command_line_args.h"

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

int init = 0;
cl_node_t *cl_dict_root = NULL;
char *args[ARGCOUNT];

static cl_node_t * cl_create_node(char c, cl_nodetype_t nodetype, cl_argument_t argtype)
{
	cl_node_t *new_node = (cl_node_t*)malloc(sizeof(cl_node_t));
	new_node->childsamt = 0;
	new_node->enabled   = argtype == AT_NONE;
	new_node->symb      = c;
	new_node->argtype   = argtype;
	new_node->nodetype  = nodetype;
	new_node->childs    = NULL;
	return new_node;
}

static cl_node_t * cl_create_dict()
{
	return cl_create_node((char)0, CLN_ROOT, AT_NONE);
}

static cl_node_t * cl_add_node(cl_node_t *parent, char c, cl_nodetype_t nodetype, cl_argument_t argtype)
{
	parent->childs = (cl_node_t**)realloc(parent->childs, ++parent->childsamt * sizeof(cl_node_t*));
	parent->childs[parent->childsamt - 1] = cl_create_node(c, nodetype, argtype);
	return parent->childs[parent->childsamt - 1];
}

static inline cl_node_t * cl_converted(cl_node_t *node, cl_nodetype_t nodetype, cl_argument_t argtype)
{
	node->nodetype = node->nodetype == CLN_NONE ? nodetype : node->nodetype;
	node->argtype = node->argtype == AT_NONE ? argtype : node->argtype;
	node->enabled = nodetype == CLN_WORDEND || node->argtype != AT_NONE;
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

static void cl_add_word(cl_node_t *parent, char *str)
{
	cl_node_t *cur_node = parent;
	for(int i = 0; i < strlen(str); i++)
		cur_node = cl_break_trought(cur_node, str[i], i == strlen(str) - 1 ? CLN_WORDEND : CLN_NONE, AT_NONE);
}

static void cl_reg(cl_node_t *parent, cl_argument_t argt, char *argv)
{
	args[argt] = (char*)malloc((strlen(argv) + 1)*sizeof(char));
	args[argt] = (char*)memcpy(args[argt], argv, (strlen(argv) + 1)*sizeof(char));
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

static void cl_recursive_get_childs(cl_node_t *parent, cl_check_t *check, int strindex)
{
#define cvals (check->values)
#define ccnt  (check->valcnt)
	if (!parent) return;
	cvals[ccnt - 1] = (char*)realloc(cvals[ccnt - 1], (strindex + 1) * sizeof(char));
	cvals[ccnt - 1][strindex - 1] = parent->symb;
	cvals[ccnt - 1][strindex] = '\0';
	if (parent->nodetype == CLN_WORDEND)
	{
		cvals = (char**)realloc(cvals, ++ccnt * sizeof(char*));
		cvals[ccnt - 1] = (char*)malloc(strlen(cvals[ccnt - 2]) + 1);
		cvals[ccnt - 1] = memcpy(cvals[ccnt - 1], cvals[ccnt - 2], strlen(cvals[ccnt - 2]) + 1);
	}
	for(int i = 0; i < parent->childsamt; i++)
		cl_recursive_get_childs(parent->childs[i], check, strindex + 1);
}

static cl_check_t * cl_get_child_words(cl_node_t *parent)
{
	cl_check_t *result = (cl_check_t*)malloc(sizeof(cl_check_t));
	result->true   = 0;
	result->valcnt = 0;
	result->values = NULL;
	if (!parent) return result;
	result->valcnt = 1;
	result->values = (char**)malloc(sizeof(char*));
	result->values[0] = (char*)malloc(sizeof(char));
	for(int i = 0; i < parent->childsamt; i++)
		cl_recursive_get_childs(parent->childs[i], result, 1);
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
	cl_add_word(cl_dict_root, argument);
}

extern void cl_split_nums(cl_check_t *check)
{
#define cvals (check->values)
#define ccnt  (check->valcnt)
	for(int iv = 0; iv < ccnt; iv++)
	{
		int cvstrlen = strlen(cvals[iv]);
		for(int i = 0; i < cvstrlen; i++)
			if(cvals[iv][i] == ',')
			{
				cvals = (char**)realloc(cvals, ++ccnt * sizeof(char*));
				cvals[ccnt - 1] = cvals[iv] + i + 1;
				cvals[iv][i] = '\0';
			}
	}
}

extern cl_check_t * cl_get_arg_res(cl_argument_t arg)
{
	cl_check_t *result = (cl_check_t*)malloc(sizeof(cl_check_t));
	result->true   = 0;
	result->valcnt = 0;
	result->values = NULL;
	if (!init) return result;
	cl_node_t *node = cl_get_word_node(cl_dict_root, args[arg]);
	result->true = node->enabled;
	result = cl_get_child_words(node);
	return result;
}

extern int cl_is_true(cl_argument_t arg)
{
	int result = cl_get_word_node(cl_dict_root, args[arg])->enabled;
	return result;
}






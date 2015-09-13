#include "dict.h"

typedef enum
{
	NT_NONE, NT_ROOT, NT_WORDEND
} nodetype_t;

typedef struct trie_node_child_t_
{
	byte_t symb;
	struct trie_node_t_ *ref;
} trie_node_child_t;

typedef struct trie_node_t_
{
	int childsamt;
	nodetype_t nodetype;
	struct trie_node_chield_t_ *childs;
} trie_node_t;

extern trie_node_t * create_node(nodetype_t nodetp)
{
	trie_node_t *new_node = (trie_node_t*)malloc(sizeof(trie_node_t));
	new_node->childs    = NULL;
	new_node->nodetype  = nodetp;
	new_node->childsamt = 0;
	return new_node;
}

static trie_node_t * add_node(trie_node_t *parent, byte_t c, nodetype_t nodetp)
{
	parent->childs = (trie_node_child_t*)realloc(parent->childs, ++parent->childsamt * sizeof(trie_node_child_t));
	trie_node_child_t ch  = parent->childs[parent->childsamt];
	ch->ref  = create_node(nodetp);
	ch->symb = c;
	return ch->ref;
}

static inline trie_node_t * converted(trie_node_t node, nodetype_t nodetp)
{
	node->nodetype = nodetp == NT_WORDEND ? NT_WORDEND : node->nodetype;
	return node;
}

static inline trie_node_t * break_trought(trie_node_t *parent, byte_t c, nodetype_t nodetp)
{
	int i;
	trie_node_child_t ch;
	for(i = 0; i < parent->childsamt; i++)
	{
		ch = parent->childs[i];
		if (ch.symb == c) break;
	}
	ch = parent->childs[i];
	return ch.symb == c && ch.ref ? converted(ch.ref, nodetp) : add_node(parent, c, nodetp);
}

extern void add_word(trie_node_t *dict, string_t *str)
{
	trie_node_t *cur_node = create_node(NT_ROOT);
	for(int i = 0; i < str->length; i++)
		cur_node = break_trought(cur_node, str->chars[i], i == str->length - 1 ? NT_WORDEND : NT_NONE);
}


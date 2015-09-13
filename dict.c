#include "dict.h"

static trie_node_t * create_node(nodetype_t nodetp)
{
	trie_node_t *new_node = (trie_node_t*)malloc(sizeof(trie_node_t));
	new_node->childs    = NULL;
	new_node->nodetype  = nodetp;
	new_node->childsamt = 0;
	return new_node;
}

extern dictionary_t * create_dict()
{
	dictionary_t *new_dict = (dictionary_t*)malloc(sizeof(dictionary_t));
	new_dict->cardinality = 0;
	new_dict->root = create_node(NT_ROOT);
	return new_dict;
}

static trie_node_t * add_node(trie_node_t *parent, byte_t c, nodetype_t nodetp)
{
	parent->childs = (trie_node_child_t*)realloc(parent->childs, ++parent->childsamt * sizeof(trie_node_child_t));
	parent->childs[parent->childsamt - 1].ref = create_node(nodetp);
	parent->childs[parent->childsamt - 1].symb = c;
	return parent->childs[parent->childsamt - 1].ref;
}

static inline trie_node_t * converted(trie_node_t *node, nodetype_t nodetp)
{
	node->nodetype = nodetp == NT_WORDEND ? NT_WORDEND : node->nodetype;
	return node;
}

static inline trie_node_t * break_trought(trie_node_t *parent, byte_t c, nodetype_t nodetp)
{
	int i;
	for(i = 0; i < parent->childsamt && parent->childs[i].symb != c; i++);
	return i < parent->childsamt && parent->childs[i].symb == c ?
		converted(parent->childs[i].ref, nodetp) : add_node(parent, c, nodetp);
}

static inline trie_node_t * trie_step(trie_node_t *parent, byte_t c)
{
	int i;
	for(i = 0; i < parent->childsamt && parent->childs[i].symb != c; i++);
	return i < parent->childsamt && parent->childs[i].symb == c ? parent->childs[i].ref : NULL;
}

extern code_t get_code(dictionary_t *dict, string_t *str)
{
	trie_node_t *cur_node = dict->root;
	for(int i = 0; i < str->length && cur_node; cur_node = trie_step(cur_node, str->chars[i++]));
	return cur_node && cur_node->nodetype == NT_WORDEND ? cur_node->code : -1;
}

extern void add_word(dictionary_t *dict, string_t *str)
{
	printf("%s\n", str->chars);
	trie_node_t *cur_node = dict->root;
	for(int i = 0; i < str->length; i++)
		cur_node = break_trought(cur_node, str->chars[i], i == str->length - 1 ? NT_WORDEND : NT_NONE);
	cur_node->code = cur_node->nodetype == NT_WORDEND ? dict->cardinality++ : -1;
}


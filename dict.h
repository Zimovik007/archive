#ifndef DICT_H_
#define DICT_H_

#include <stdlib.h>
#include "types.h"
#include "mystring.h"

#define NO_WORD (NULL)

typedef enum
{
	NT_NONE, NT_ROOT, NT_WORDEND
} nodetype_t;

typedef struct trie_node_child_t
{
	byte_t symb;
	struct trie_node_t *ref;
} trie_node_child_t;

typedef struct trie_node_t
{
	int childsamt;
	code_t code;
	nodetype_t nodetype;
	struct trie_node_child_t *childs;
} trie_node_t;

typedef struct row_t
{
	code_t code;
	string_t *word;
} row_t;

typedef struct trie_dictionary_t
{
	int cardinality;
	trie_node_t *root;
	row_t *words;
	arch_mode mode;
} dictionary_t;

extern dictionary_t * create_dict(arch_mode);

extern code_t get_code(dictionary_t *, string_t *);

extern string_t * get_word(dictionary_t *, code_t);

extern void add_word(dictionary_t *, string_t *);

#endif /* DICT_H_ */

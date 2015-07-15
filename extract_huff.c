#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compress.h"
#include "extract_huff.h"

typedef
	struct ext_huff_node_t
	{
		unsigned char c;
		struct ext_huff_node_t *left, *right;
	} ext_huff_node_t;
	
static ext_huff_node_t * ext_tree_step(ext_huff_node_t *parent, int bit)
{
	return bit ? parent->right : parent->left;
}

static ext_huff_node_t * ext_create_huff_node(const unsigned char c)
{
	ext_huff_node_t *new_node = (ext_huff_node_t*)malloc(sizeof(ext_huff_node_t));
	new_node->c = c;
	new_node->left = NULL;
	new_node->right = NULL;
	return new_node;
}

static ext_huff_node_t * ext_add_huff_node(ext_huff_node_t *parent, int bit, unsigned char c)
{
	ext_huff_node_t *result = NULL;
	if (bit == 0)
	{
		if (!parent->left)
			parent->left = ext_create_huff_node(c);			
		result = parent->left;
	}
	else 
	if (bit == 1)
	{
		if (!parent->right)
			parent->right = ext_create_huff_node(c);
		result = parent->right;
	}
	return result;
}

static void ext_build_huff_tree(cano_huff_t *codes, ext_huff_node_t *root)
{
	int i, j, bit;
	ext_huff_node_t *cur_node;
	for(i = 0; i < CHARS_NUM; i++)
	{
		cur_node = root;
		for(j = codes[i].length - 1; j >= 0; j--)
		{
			bit = (codes[i].code & (1 << j)) != 0;
			cur_node = ext_add_huff_node(cur_node, bit, j == 0 ? codes[i].c : (unsigned char)0);
		}	
	}
}

extern void extract_huffman(FILE *fin, FILE *fout)
{
	ext_huff_node_t *root = ext_create_huff_node((unsigned char)0), *cur_node;
	cano_huff_t *codes = (cano_huff_t*)malloc(CHARS_NUM * sizeof(cano_huff_t));
	int i;	
	for(i = 0; i < CHARS_NUM; i++)
	{
		fscanf(fin, "%d ", &codes[i].length);
		codes[i].c = (unsigned char)i;
		codes[i].code = 0;	
	}
	generate_codes(codes);
	ext_build_huff_tree(codes, root);
	
	unsigned char c = 0;
	cur_node = root;
	while (!feof(fin) && cur_node)
	{
		fscanf(fin, "%c", &c);
		for(i = 1; i <= 128; i = i << 1)
		{
			cur_node = ext_tree_step(cur_node, (c & i) == i);
			if (!cur_node) break;
			if (!cur_node->left && !cur_node->right)
			{
				fprintf(fout, "%c", cur_node->c);
				cur_node = root;
			}
		}
	}
}











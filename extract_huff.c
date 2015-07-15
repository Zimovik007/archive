#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compress.h"
#include "extract_huff.h"

typedef
	struct ext_huff_node_t
	{
		char c;
		struct ext_huff_node_t *left, *right;
	} ext_huff_node_t;
	
static ext_huff_node_t * ext_tree_step(ext_huff_node_t *parent, char bit)
{
	if (bit == '0')
		return parent->left;
	else
	if (bit == '1')
		return parent->right;
	else 
		return NULL;
}

static ext_huff_node_t * ext_create_huff_node(const char c)
{
	ext_huff_node_t *new_node = (ext_huff_node_t*)malloc(sizeof(ext_huff_node_t));
	new_node->c = c;
	new_node->left = new_node->right = NULL;
	return new_node;
}

static ext_huff_node_t * ext_add_huff_node(ext_huff_node_t *parent, char bit, char c)
{
	ext_huff_node_t *result;
	if (bit == '0')
	{
		if (!parent->left)
			parent->left = ext_create_huff_node(c);			
		result = parent->left;
	}
	else 
	if (bit == '1')
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
		for(j = 0; j < codes[i].length; j++)
		{
			bit = codes[i].code & (1 << j);
			cur_node = ext_add_huff_node(cur_node, bit, codes[i].c);			
		}	
	}	
}

extern void extract_huffman(FILE *fin)
{
	rewind(fin);
	ext_huff_node_t *root = ext_create_huff_node((char)0), *cur_node;
	cano_huff_t *codes = (cano_huff_t*)malloc(CHARS_NUM * sizeof(cano_huff_t));
	int i;	
	for(i = 0; i < CHARS_NUM; i++)
	{
		fscanf(fin, "%d ", &codes[i].length);
		codes[i].c = i;		
	}
	generate_codes(codes);
	ext_build_huff_tree(codes, root);
	FILE *fout = fopen("0900909009", "w");
	
	char c;
	cur_node = root;
	while (fscanf(fin, "%c", &c) != EOF && cur_node)
	{
		printf("!\n");
		for(i = 1; i <= 128; i *= 2)
		{
			cur_node = (c & i) == i ? ext_tree_step(cur_node, '1') : ext_tree_step(cur_node, '0');
			if (!cur_node) break;
			if (!cur_node->left && !cur_node->right)
			{
				fprintf(fout, "%c", cur_node->c);
				cur_node = root;				
			}
		}
	}
	fclose(fout);
}

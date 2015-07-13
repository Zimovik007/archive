#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef
	struct huff_node_t
	{
		char c;
		struct huff_node_t *left, *right;
	} huff_node_t;
	
static huff_node_t * find_parent(huff_node_t *parent, char bit)
{
	if (bit == '0')
	{
		if (parent->left)
			return find_parent(parent->left, bit);
		else
			return parent;		
	}
	else if (bit == '1')
	{
		if (parent->right)
			return find_parent(parent->right, bit);
		else
			return parent;
	}
	else return NULL;
}

static huff_node_t * create_huff_node(const char c)
{
	huff_node_t *new_node = (huff_node_t*)malloc(sizeof(huff_node_t));
	new_node->c = c;
	new_node->left = new_node->right = NULL;
	return new_node;
}

static void add_huff_node(huff_node_t *root, char bit, char c)
{
	huff_node_t *parent = find_parent(root, bit);
	if (bit == '0')
		parent->left = create_huff_node(c);
	else if (bit == '1')
		parent->right = create_huff_node(c);
}

extern void extract_huffman(FILE *fin)
{
	double b_count;
	fscanf(fin, "%lf\n", &b_count);
	long unsigned int bits_count = (int)b_count;
	huff_node_t *root = (huff_node_t*)malloc(sizeof(huff_node_t));
	root->left = root->right = NULL;
	root->c = (char)0;
	int key_leng = 1, i;
	char c, bit;
	while (key_leng != 0)
	{
		fscanf(fin, "%d %c", &key_leng, &c);
		for(i = 0; i < key_leng; i++)
		{
			fscanf(fin, "%c", &bit);
			add_huff_node(root, bit, c);			
		}
	}		
}

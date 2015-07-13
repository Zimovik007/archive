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
		return parent->left ? find_parent(parent->left, bit) : parent;
	else 
	if (bit == '1')
		return parent->right ? find_parent(parent->right, bit) : parent;
	else 
		return NULL;
}

static huff_node_t * create_huff_node(const char c)
{
	huff_node_t *new_node = (huff_node_t*)malloc(sizeof(huff_node_t));
	new_node->c = c;
	new_node->left = new_node->right = NULL;
	return new_node;
}

static huff_node_t * add_huff_node(huff_node_t *root, char bit, char c)
{
	huff_node_t *parent = find_parent(root, bit), *result;
	if (bit == '0')
		result = parent->left ? parent->left : parent->left = create_huff_node(c);
	else if (bit == '1')
		result = parent->right ? parent->right : parent->right = create_huff_node(c);
	return result;
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
	huff_node_t *cur_node = root;
	while (key_leng != 0)
	{
		cur_node = root;
		fscanf(fin, "%d %c", &key_leng, &c);
		for(i = 0; i < key_leng; i++)
		{
			fscanf(fin, "%c", &bit);
			cur_node = add_huff_node(cur_node, bit, c);			
		}
	}
		
}

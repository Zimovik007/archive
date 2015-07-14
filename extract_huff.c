#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef
	struct ext_huff_node_t
	{
		char c;
		struct ext_huff_node_t *left, *right;
	} ext_huff_node_t;
	
static ext_huff_node_t * find_parent(ext_huff_node_t *parent, char bit)
{
	if (bit == '0')
		return parent->left ? find_parent(parent->left, bit) : parent;
	else 
	if (bit == '1')
		return parent->right ? find_parent(parent->right, bit) : parent;
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

static ext_huff_node_t * add_huff_node(ext_huff_node_t *root, char bit, char c)
{
	ext_huff_node_t *parent = find_parent(root, bit), *result;
	if (bit == '0')
	{
		if (parent->left)
			result = parent->left;
		else 
			result = parent->left = ext_create_huff_node(c);
	}
	else 
	if (bit == '1')
	{
		if (parent->right)
			result = parent->right;
		else
			result = parent->right = ext_create_huff_node(c);
	}
	return result;
}

extern void extract_huffman(FILE *fin)
{
	double b_count;
	rewind(fin);
	FILE *fout = fopen("bovodoso", "w");
	fscanf(fin, "%lf\n", &b_count);
	long unsigned int bits_count = (int)b_count;
	ext_huff_node_t *root = (ext_huff_node_t*)malloc(sizeof(ext_huff_node_t));
	root->left = root->right = NULL;
	root->c = (char)0;
	int key_leng = 1, i;
	char c, bit;
	ext_huff_node_t *cur_node = root;
	while (key_leng != 0)
	{
		cur_node = root;
		fscanf(fin, "%d %c", &key_leng, &c);
		for(i = 0; i < key_leng; i++)
		{
			fscanf(fin, "%c", &bit);
			cur_node = add_huff_node(cur_node, bit, c);	
		}
		fscanf(fin, "\n");
	}
	long unsigned int k = 0;
	while (fscanf(fin, "%c", &c) != EOF)
	{
		cur_node = root;
		for(i = 128; i >= 1; i /= 2)
		{
			cur_node = (c & i) == i ? find_parent(cur_node, '1') : find_parent(cur_node, '0');
			if (!cur_node->left && !cur_node->right)
			{
				fprintf(fout, "%c", cur_node->c);
				cur_node = root;				
			}
			if (++k == bits_count)
			{
				fclose(fout);
				return;
			}
		}
	}
	//printf("%ld", sizeof(int));
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef
	struct huff_node_t
	{
		char c;
		struct huff_node_t *left, *right;
	} huff_node_t;
	
static void find(huff_node_t *parent, char bit)
{
	
	
}

static void add_huff_node(huff_node_t *root, char bit, char c)
{
	
	
}

extern void extract_huffman(FILE *fin)
{
	double b_count;
	fscanf(fin, "%lf\n", &b_count);
	long unsigned int bits_count = (int)b_count;
	huff_node_t *root = (huff_node_t)malloc(sizeof(huff_node_t));
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

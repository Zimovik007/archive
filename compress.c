#include <stdio.h>
#include <stdlib.h>

#define CHARS_NUM (256)

typedef
	struct huff_node_t
	{
		int frequency;
		char c;
		struct huff_node_t *left, *right;
	} huff_node_t;

int frequency[CHARS_NUM];

static void count_frequency(FILE *fin)
{
	char c, i;
	for(i = 0; i < CHARS_NUM; i++) frequency[(int)i] = 0;
	while (fscanf(fin, "%c", &c)) ++frequency[(int)c];	
}

static huff_node_t * create_huff_node(const char c, const int freq)
{
	huff_node_t *new_node = (huff_node_t*)malloc(sizeof(huff_node_t));
	new_node->c = c;
	new_node->frequency = freq;
	new_node->left = new_node->right = NULL;
	return new_node;
}

static huff_node_t * build_huff_tree()
{
	
	
	return NULL;
}

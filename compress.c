#include <stdio.h>
#include <stdlib.h>
#include "priority_queue.c"

#define CHARS_NUM (256)

typedef
	struct huff_node_t
	{
		int frequency;
		char c;
		struct huff_node_t *left, *right;
	} huff_node_t;

int frequency[CHARS_NUM];
huff_node_t *nodes = NULL;

static void count_frequency(FILE *fin)
{
	char c, i;
	for(i = 0; i < CHARS_NUM; i++) frequency[(int)i] = 0;
	while (fscanf(fin, "%c", &c)) ++frequency[(int)c];	
}

static huff_node_t * create_huff_node(const char c, const int frequency, 
	huff_node_t *left, huff_node_t *right)
{
	huff_node_t *new_node = (huff_node_t*)malloc(sizeof(huff_node_t));
	new_node->c = c;
	new_node->frequency = frequency;
	new_node->left = left;
	new_node->right = right;
	return new_node;
}

static huff_node_t * build_huff_tree()
{
	int i, nodes_count = 0;
	queue_t *queue = create_queue(sizeof(huff_node_t));
	for(i = 0; i < CHARS_NUM; i++)
	{
		if (frequency[i] > 0)
			queue_insert(queue, frequency[i], create_huff_node(char(i), frequency[i], NULL, NULL));	
	}
	huff_node_t *left = NULL, *right = NULL;
	do
	{
		left = (huff_node_t*)queue_pop(queue);
		right = (huff_node_t*)queue_pop(queue);
		if (!right) break;
		queue_insert(queue, create_huff_node(char(0), left->frequency + right->frequency, left, right));
	} while (left && right);
	return NULL;
}

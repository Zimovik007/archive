#include <stdio.h>
#include <stdlib.h>
#include "priority_queue.c"

#define CHARS_NUM (256)

typedef
	struct huff_node_t
	{
		unsigned long int frequency;
		char c;
		struct huff_node_t *left, *right;
	} huff_node_t;

static void count_frequency(FILE *fin, unsigned long int *frequency)
{
	char c;
	int i;
	for(i = 0; i < CHARS_NUM; i++) frequency[i] = 0;
	while (fscanf(fin, "%c", &c) != EOF) ++frequency[(int)c];
}

static huff_node_t * create_huff_node(const char c, const int freq, 
	huff_node_t *left, huff_node_t *right)
{
	huff_node_t *new_node = (huff_node_t*)malloc(sizeof(huff_node_t));
	new_node->c = c;
	new_node->frequency = freq;
	new_node->left = left;
	new_node->right = right;
	return new_node;
}

static huff_node_t * build_huff_tree(unsigned long int *frequency)
{
	int i;
	queue_t *queue = create_queue(sizeof(huff_node_t));
	for(i = 0; i < CHARS_NUM; i++)
	{
		if (frequency[i] > 0)
			queue_insert(queue, frequency[i], create_huff_node((char)i, frequency[i], NULL, NULL));	
	}	
	
	huff_node_t *left = NULL, *right = NULL;
	do
	{
		left = (huff_node_t*)queue_pop(queue);
		right = (huff_node_t*)queue_pop(queue);
		if (!right || !left) break;
		queue_insert(queue, left->frequency + right->frequency, 
			create_huff_node((char)0, left->frequency + right->frequency, left, right));
	} while (left && right);
	
	return left;
}

int code_stack[1000], stack_size = 0;
static void _codes(huff_node_t *parent, int **codes, int *lengths)
{
	if (!parent) return;
	if (parent->left)
	{
		code_stack[stack_size++] = 0;
		_codes(parent->left, codes, lengths);
		--stack_size;
	}
	else
	{
		memcpy(codes[(int)parent->c], code_stack, stack_size*sizeof(int));
		lengths[(int)parent->c] = stack_size;
		return;
	}
	
	if (parent->right)
	{
		code_stack[stack_size++] = 1;
		_codes(parent->right, codes, lengths);
		--stack_size;
	}
	else
	{
		memcpy(codes[(int)parent->c], code_stack, stack_size*sizeof(int));
		lengths[(int)parent->c] = stack_size;
		return;
	}	
}

extern void compress_huffman(FILE *fin)
{
	unsigned long int *frequency = (unsigned long int*)calloc(CHARS_NUM, sizeof(unsigned long int));
	count_frequency(fin, frequency);
	huff_node_t *root = build_huff_tree(frequency);
	int 
		**codes = (int**)calloc(CHARS_NUM, sizeof(int*)), 
		*code_lengths = (int*)calloc(CHARS_NUM, sizeof(int));
	_codes(root, codes, code_lengths);
}

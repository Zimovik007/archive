#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priority_queue.h"
#include "compress.h"

typedef
	struct huff_node_t
	{
		unsigned int frequency;
		byte_t c;
		struct huff_node_t *left, *right;
	} huff_node_t;

static void count_frequency(FILE *orig, filesize_t *frequency, filesize_t *orig_size)
{
	rewind(orig);
	byte_t c;
	for(int i = 0; i < CHARS_NUM; i++) frequency[i] = 0;
	while (!feof(orig))
	{
		if (fscanf(orig, "%c", &c) <= 0) break;
		++frequency[(int)c];
		++*orig_size;
	}
}

static huff_node_t * create_huff_node(const byte_t c, const unsigned int freq,
	huff_node_t *left, huff_node_t *right)
{
	huff_node_t *new_node = (huff_node_t*)malloc(sizeof(huff_node_t));
	new_node->c = c;
	new_node->frequency = freq;
	new_node->left = left;
	new_node->right = right;
	return new_node;
}

static huff_node_t * build_huff_tree(filesize_t *frequency)
{
	queue_t *queue = create_queue(sizeof(huff_node_t));
	for(int i = 0; i < CHARS_NUM; i++)
	{
		if (frequency[i] > 0)
			queue_insert(queue, frequency[i], create_huff_node((byte_t)i, frequency[i], NULL, NULL));
	}

	huff_node_t *left = NULL, *right = NULL;
	do
	{
		left = (huff_node_t*)queue_pop(queue);
		right = (huff_node_t*)queue_pop(queue);
		if (!right || !left) break;
		queue_insert(queue, left->frequency + right->frequency,
			create_huff_node((byte_t)0, left->frequency + right->frequency, left, right));
	} while (left && right);

	return left;
}

int stack_size = 0;
static void count_lengths_codes(huff_node_t *parent, cano_huff_t *codes)
{
	if (!parent) return;
	if (parent->left)
	{
		++stack_size;
		count_lengths_codes(parent->left, codes);
		--stack_size;
	}
	else if (!parent->right)
	{
		codes[(int)parent->c].length = stack_size;
		return;
	}
	if (parent->right)
	{
		++stack_size;
		count_lengths_codes(parent->right, codes);
		--stack_size;
	}
	else if (!parent->left)
	{
		codes[(int)parent->c].length = stack_size;
		return;
	}
}

int compare_lengths_stable(const void *a, const void *b)
{
	cano_huff_t A = *((cano_huff_t*)a), B = *((cano_huff_t*)b);
	if (A.length > B.length) return 1;
	else if (A.length < B.length) return -1;
	else if (A.c > B.c) return 1;
	else if (A.c < B.c) return -1;
	else return 0;
}

int compare_chars_alphabet(const void *a, const void *b)
{
	cano_huff_t A = *((cano_huff_t*)a), B = *((cano_huff_t*)b);
	if (A.c > B.c) return 1;
	else if (A.c < B.c) return -1;
	else return 0;
}

extern void generate_codes(cano_huff_t *codes)
{
	int i;
	qsort(codes, CHARS_NUM, sizeof(cano_huff_t), compare_lengths_stable);
	for(i = 1; i < CHARS_NUM; i++)
	{
		if (codes[i - 1].length == 0) continue;
		if (codes[i].length != codes[i - 1].length)
			codes[i].code = (codes[i - 1].code + 1) << (codes[i].length - codes[i - 1].length);
		else
			codes[i].code = codes[i - 1].code + 1;
	}
	qsort(codes, CHARS_NUM, sizeof(cano_huff_t), compare_chars_alphabet);
}

static void save_tree(FILE *archf, cano_huff_t *codes, filesize_t *archf_size)
{
	for(int i = 0; i < CHARS_NUM; i++)
		fprintf(archf, "%c", (byte_t)codes[i].length);
	*archf_size += (filesize_t)CHARS_NUM;
}

static void codify(FILE *orig, cano_huff_t *codes, FILE *archf, filesize_t *archf_size)
{
	rewind(orig);
	save_tree(archf, codes, archf_size);
	byte_t print_char = 0;
	int    k          = 0;
	int    bit        = 0;
	int    j;
	byte_t c;

	while (!feof(orig))
	{
		if (fscanf(orig, "%c", &c) <= 0) break;
		for(j = CHARS_NUM - 1; codes[j].c != c; j--);
		for(int i = codes[j].length - 1; i >= 0; i--)
		{
			bit = codes[j].code & (1 << i);
			bit = !!bit;
			print_char = print_char | (bit << k);
			if (++k == 8)
			{
				fprintf(archf, "%c", print_char);
				print_char = (byte_t)0;
				k = 0;
				++*archf_size;
			}
		}
	}
	if (k > 0)
	{
		fprintf(archf, "%c", print_char);
		++*archf_size;
	}
}

extern void compress_huffman(FILE *orig, FILE *archf, filesize_t *orig_size, filesize_t *archf_size)
{
	*archf_size = 0;
	*orig_size  = 0;
	filesize_t *frequency = (filesize_t*)calloc(CHARS_NUM, sizeof(filesize_t));

	count_frequency(orig, frequency, orig_size);
	huff_node_t *root  = build_huff_tree(frequency);
	cano_huff_t *codes = (cano_huff_t*)malloc(CHARS_NUM * sizeof(cano_huff_t));

	for(int i = 0; i < CHARS_NUM; i++)
	{
		codes[i].c = (byte_t)i;
		codes[i].length = 0;
		codes[i].code = 0;
	}

	count_lengths_codes(root, codes);
	if (root && !root->left && !root->right)
		codes[(int)root->c].length = 1;
	generate_codes(codes);

	codify(orig, codes, archf, archf_size);
}

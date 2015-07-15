#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priority_queue.h"
#include "compress.h"

typedef
	struct huff_node_t
	{
		unsigned int frequency;
		unsigned char c;
		struct huff_node_t *left, *right;
	} huff_node_t;

static void count_frequency(FILE *fin, unsigned int *frequency)
{
	unsigned char c;
	int i;
	for(i = 0; i < CHARS_NUM; i++) frequency[i] = 0;
	while (!feof(fin)) 
	{
		fscanf(fin, "%c", &c);
		++frequency[(int)c];
	}
}

static huff_node_t * create_huff_node(const unsigned char c, const unsigned int freq, 
	huff_node_t *left, huff_node_t *right)
{
	huff_node_t *new_node = (huff_node_t*)malloc(sizeof(huff_node_t));
	new_node->c = c;
	new_node->frequency = freq;
	new_node->left = left;
	new_node->right = right;
	return new_node;
}

static huff_node_t * build_huff_tree(unsigned int *frequency)
{
	int i;
	queue_t *queue = create_queue(sizeof(huff_node_t));
	for(i = 0; i < CHARS_NUM; i++)
	{
		if (frequency[i] > 0)
			queue_insert(queue, frequency[i], create_huff_node((unsigned char)i, frequency[i], NULL, NULL));	
	}
	
	huff_node_t *left = NULL, *right = NULL;
	do
	{
		left = (huff_node_t*)queue_pop(queue);
		right = (huff_node_t*)queue_pop(queue);
		if (!right || !left) break;
		queue_insert(queue, left->frequency + right->frequency, 
			create_huff_node((unsigned char)0, left->frequency + right->frequency, left, right));
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
		{
			codes[i].code = (codes[i - 1].code + 1) << 1;
		}
		else
			codes[i].code = codes[i - 1].code + 1;
	}
	qsort(codes, CHARS_NUM, sizeof(cano_huff_t), compare_chars_alphabet);
}

static void save_tree(FILE *fout, cano_huff_t *codes)
{	
	int i;
	for(i = 0; i < CHARS_NUM; i++)
	{
		fprintf(fout, "%d ", codes[i].length);
	}
}

static void codify(FILE *fin, huff_node_t *root, cano_huff_t *codes, FILE *fout)
{
	rewind(fin);
	save_tree(fout, codes);
	unsigned char c, prin_c = (unsigned char)0;
	int i, k = 0, bit, j;
	
	while (fscanf(fin, "%c", &c) != EOF)
	{
		for(j = CHARS_NUM - 1; codes[j].c != c; j--);
		for(i = codes[j].length - 1; i >= 0; i--)
		{
			bit = codes[j].code & (1 << i);
			bit = !(!bit);
			
			prin_c = prin_c | (bit << k);						
			if (++k == 8)
			{
				fprintf(fout, "%c", prin_c);
				prin_c = (unsigned char)0;
				k = 0;
			}
		}
	}
	if (k > 0) fprintf(fout, "%c", prin_c);
	fclose(fout);
}

extern FILE * compress_huffman(FILE *fin, char ArchiveName[200])
{
	rewind(fin);
	FILE* fout = fopen(ArchiveName, "wb");
	unsigned int *frequency = (unsigned int*)calloc(CHARS_NUM, sizeof(unsigned int));
	count_frequency(fin, frequency);
	int i;	
	
	huff_node_t *root = build_huff_tree(frequency);
	cano_huff_t *codes = (cano_huff_t*)malloc(CHARS_NUM * sizeof(cano_huff_t));

	for(i = 0; i < CHARS_NUM; i++)
	{
		codes[i].c = (unsigned char)i;
		codes[i].length = 0;
		codes[i].code = 0;
	}
	
	count_lengths_codes(root, codes);

	generate_codes(codes);
	
	codify(fin, root, codes, fout);
	
	return fout;
}

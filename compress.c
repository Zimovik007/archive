#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priority_queue.c"

#define CHARS_NUM (256)
#define ASCII_0 (48)

typedef
	struct huff_node_t
	{
		unsigned long int frequency;
		char c;
		struct huff_node_t *left, *right;
	} huff_node_t;
	
typedef
	struct cano_huff_t
	{
		int length;
		unsigned char c;
	} cano_huff_t;

static void count_frequency(FILE *fin, unsigned long int *frequency)
{
	char c;
	int i;
	for(i = 0; i < CHARS_NUM; i++) frequency[i] = 0;
	while (fscanf(fin, "%c", &c) != EOF) 
		++frequency[(int)c + CHARS_NUM/2];		
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
			queue_insert(queue, frequency[i], create_huff_node((char)(i - CHARS_NUM/2), frequency[i], NULL, NULL));	
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

int code_stack[10000], stack_size = 0;
//static void _codes(huff_node_t *parent, int **codes, cano_huff_t *lengths)
static void _codes(huff_node_t *parent, cano_huff_t *lengths)
{
	if (!parent) return;
	if (parent->left)
	{
		code_stack[stack_size++] = 0;
		_codes(parent->left, lengths);
		--stack_size;
	}
	else if (!parent->right)
	{
		//codes[parent->c + CHARS_NUM/2] = (int*)malloc(stack_size*sizeof(int));
		//memcpy(codes[parent->c + CHARS_NUM/2], code_stack, stack_size*sizeof(int));
		lengths[parent->c + CHARS_NUM/2].length = stack_size;
		return;
	}	
	if (parent->right)
	{
		code_stack[stack_size++] = 1;
		_codes(parent->right, lengths);
		--stack_size;
	}
	else if (!parent->left)
	{
		//codes[parent->c + CHARS_NUM/2] = (int*)malloc(stack_size*sizeof(int));
		//memcpy(codes[parent->c + CHARS_NUM/2], code_stack, stack_size*sizeof(int));
		lengths[parent->c + CHARS_NUM/2].length = stack_size;
		printf("%d\n", stack_size);
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

static void save_tree(FILE *fout, unsigned long int *codes, cano_huff_t *lengths)
{	
	int i, j, lng;
	char c;
	for(i = 0; i < CHARS_NUM; i++)
	{
		lng = lengths[i].length;
		if (lng <= 0) continue;
		c = (char)(i - CHARS_NUM/2);
		fprintf(fout, "%d %c", lng, c);
		//for(j = 0; j < lng; j++)
			//fprintf(fout, "%c", (char)(codes[i][j] + ASCII_0));
		fprintf(fout, "\n");		
	}
	fprintf(fout, "0 #\n");
}

static void codify(FILE *fin, huff_node_t *root, unsigned long int *codes, cano_huff_t *lengths)
{
	rewind(fin);
	FILE *fout = fopen("ababaca", "w");
	fprintf(fout, "00000000000000000000\n");
	save_tree(fout, codes, lengths);
	char c, prin_c = (char)0;
	int i, index, k;
	long unsigned int bits_count = 0;
	while (fscanf(fin, "%c", &c) != EOF)
	{
		index = c + CHARS_NUM/2;
		for(i = 0; i < lengths[index].length; i++)
		{
			prin_c = (prin_c << 1) | codes[index][i]; 
			prin_c = prin_c | (codes[index][i] << k);			
			++bits_count;
			if (++k == 8)
			{
				fprintf(fout, "%c", prin_c);
				prin_c = (char)0;
				k = 0;				
			}
		}
	}
	rewind(fout);
	fprintf(fout, "%lu.", bits_count);
	fclose(fout);
}

extern void compress_huffman(FILE *fin)
{
	unsigned long int *frequency = (unsigned long int*)calloc(CHARS_NUM, sizeof(unsigned long int));
	count_frequency(fin, frequency);
	huff_node_t *root = build_huff_tree(frequency);
	unsigned long int *codes = (unsigned long int*)malloc(CHARS_NUM * sizeof(unsigned long int));
	cano_huff_t *code_lengths = (cano_huff_t*)calloc(CHARS_NUM, sizeof(cano_huff_t));
	int i;
	for(i = 0; i < CHARS_NUM; i++)
		code_lengths[(int)i].c = i;
	_codes(root, code_lengths);
	qsort(code_lengths, CHARS_NUM, sizeof(cano_huff_t), compare_lengths_stable);
	for(i = 0; i < CHARS_NUM; i++)
		printf("%c %d\n", code_lengths[i].c, code_lengths[i].length);
	codes[code_lengths[0].c + CHARS_NUM/2] = 0;
	for(i = 1; i < CHARS_NUM - 1; i++)
	{
		if (code_lengths[i].length == code_lengths[i - 1].length)
			codes[code_lengths[i].c + CHARS_NUM/2] = codes[code_lengths[i - 1].c + CHARS_NUM/2] + 1;
		else
			codes[code_lengths[i].c + CHARS_NUM/2] = (codes[code_lengths[i - 1].c + CHARS_NUM/2] << 1) + 1;
	}
		
	codify(fin, root, codes, code_lengths);			
}

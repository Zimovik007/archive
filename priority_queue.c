#include <stdio.h>
#include <stdlib.h>

typedef
	struct queue_t
	{
		int *priority, el_size, count;
		void **elements;
	} queue_t;
	
queue_t * create_queue(int element_size)
{
	queue_t *new_q = (queue_t*)malloc(sizeof(queue_t));
	new_q->priority = (int*)malloc(sizeof(int));
	new_q->elements = malloc(sizeof(void*));
	new_q->el_size = element_size;
	new_q->count = 0;
	return new_q;
}	

void swap_queue_elements(queue_t *que, int a, int b)
{
	void *t_el = que->elements[a];
	que->elements[a] = que->elements[b];
	que->elements[b] = t_el;
	int t_prior = que->priority[a];
	que->priority[a] = que->priority[b];
	que->priority[b] = t_prior;		
}

static void queue_sift_up(queue_t *que, int new_id)
{
	int i;
	for(i  = new_id; i > 1; i /= 2)
		if (que->priority[i] < que->priority[i/2])
			swap_queue_elements(que, i, i/2);
		else break;
}

void queue_insert(queue_t *que, int priority, void *element)
{
	int count = ++que->count;
	que->priority = (int*)realloc(que->priority, (count + 1)*sizeof(int));
	que->elements = (void**)realloc(que->elements, (count + 1)*sizeof(void*));
	que->priority[count] = priority;
	que->elements[count] = element;
	queue_sift_up(que, count);
}

static void queue_sift_down(queue_t *que)
{
	int i, min_ch_pr = 1, count = que->count;
	for(i = 1; ; i = min_ch_pr)
	{
		if (2*i + 1 <= count)
			min_ch_pr = que->priority[2*i] < que->priority[2*i + 1] ? 2*i : 2*i + 1;
		else if (2*i == count)
			min_ch_pr = 2*i;
		else return;
		
		if (que->priority[i] > que->priority[min_ch_pr])
			swap_queue_elements(que, i, min_ch_pr);
		else break;	
	}
}

void * queue_pop(queue_t *que)
{
	if (que->count < 1) return NULL;
	int count = --que->count;
	void *result = que->elements[1];
	swap_queue_elements(que, 1, count + 1);
	que->elements = realloc(que->elements, (count + 1)*sizeof(void*));
	que->priority = (int*)realloc(que->priority, (count + 1)*sizeof(int));
	queue_sift_down(que);
	return result;	
}


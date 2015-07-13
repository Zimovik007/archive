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
	int i, *priors = que->priority;
	for(i  = new_id; i > 1; i /= 2)
		if (priors[i] < priors[i/2])
			swap_queue_elements(que, i, i/2);
		else break;
}

void queue_insert(queue_t *que, int priority, void *element)
{
	int count = ++que->count, *priors = que->priority;
	void **els = que->elements;
	que->priority = (int*)realloc(priors, (count + 1)*sizeof(int));
	que->elements = (void**)realloc(els, (count + 1)*sizeof(void*));
	priors[count] = priority;
	els[count] = element;
	queue_sift_up(que, count);
}

static void queue_sift_down(queue_t *que)
{
	int i, *priors = que->priority, min_ch_pr = 1, count = que->count;
	for(i = 1; ; i = min_ch_pr)
	{
		if (2*i + 1 >= count)
			min_ch_pr = priors[2*i] < priors[2*i + 1] ? 2*i : 2*i + 1;
		else if (2*i == count)
			min_ch_pr = 2*i;
		else return;
		
		if (priors[i] > priors[min_ch_pr])
			swap_queue_elements(que, i, min_ch_pr);
		else break;		
	}
}

void * queue_pop(queue_t *que)
{
	int count = --que->count, *priors = que->priority;
	void **els = que->elements, *result = que->elements[1];
	que->elements[1] = els[count + 1];
	que->priority[1] = priors[count + 1];
	que->elements = realloc(els, (count + 1)*sizeof(void*));
	que->priority = (int*)realloc(priors, (count + 1)*sizeof(int));
	queue_sift_down(que);
	return result;	
}


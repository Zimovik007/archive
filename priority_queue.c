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
	que->priority = (int*)realloc(priors, count*sizeof(int));
	que->elements = (void**)realloc(els, count*sizeof(void*));
	priors[count - 1] = priority;
	els[count - 1] = element;
	queue_sift_up(que, count - 1);
}

//void 


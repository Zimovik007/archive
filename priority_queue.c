#include <stdio.h>
#include <stdlib.h>

typedef
	struct queue_t
	{
		int *priority, el_size, count;
		void **queue;
	} queue_t;
	
queue_t * create_queue(int element_size)
{
	queue_t *new_q = (queue_t*)malloc(sizeof(queue_t));
	new_q->priority = (int*)malloc(sizeof(int));
	new_q->queue = malloc(sizeof(void*));
	new_q->el_size = element_size;
	new_q->count = 0;
	return new_q;	
}	

void insert(queue_t *que, int priority, void *element)
{
	que->priority = (int*)realloc(que->priority, ++que->count*sizeof(int));
	que->queue = (void**)realloc(que->queue, que->count*sizeof(void*));
	
}


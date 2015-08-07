#ifndef PRIORITY_SEQ_H
#define PRIORITY_SEQ_H

#include <stdlib.h>
#include "types.h"

typedef
	struct queue_t
	{
		int *priority, el_size, count;
		void **elements;
	} queue_t;

extern queue_t * create_queue(int);

extern void queue_insert(queue_t *, int, void *);

extern void * queue_pop(queue_t *);

#endif

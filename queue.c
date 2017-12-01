#include "queue.h"

void initQueue(QueueType* q, int size)
{
	q->base = (void**)malloc(sizeof(void*) * size);
	if (q->base == NULL)
	{
		printf("Queue Malloc error\n");
	}
	q->front = q->rear = -1;
	q->length = 0;
	q->size = size;
}

void enQueue(QueueType* q, void* x)
{
	if (q->front == -1 && ((q->rear + 1) == q->size))
	{
		printf("Queue is full!\n");
	}
	else if (((q->rear + 1) % q->size) == q->front)
	{
		printf("Queue is full!\n");
	}
	else
	{
		q->rear = (q->rear + 1) % (q->size);
		q->base[q->rear] = x;
		q->length++;
	}
}

void* deQueue(QueueType* q)
{
	void *tmp = NULL;
	if (q->front == q->rear)
	{
		printf("Queue is empty\n");
		q->length = 0;
	}
	else
	{
		tmp = q->base[(q->front + 1) % (q->size)];
		q->front = (q->front + 1) % (q->size);
		q->length--;
	}
	return tmp;
}
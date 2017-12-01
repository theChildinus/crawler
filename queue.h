#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
typedef struct queuetype
{
	void **base;
	int front, rear;
	int size;
	int length;
}QueueType;

void initQueue(QueueType* q, int size);
void enQueue(QueueType* q, void* x);
void* deQueue(QueueType* q);

#endif

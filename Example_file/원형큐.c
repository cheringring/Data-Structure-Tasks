/*
 *	cqueue.c
 */
#include <stdio.h>

#define MAX_QUEUE_SIZE 100
typedef int QueueObject;
QueueObject  queue[MAX_QUEUE_SIZE]; 
int  front = 0;
int  rear = 0; 



int isEmpty()
{
	return (front == rear);
}

int isFull()
{
	return ((rear+1)%MAX_QUEUE_SIZE == front);
}

void addq(QueueObject item)
{ 
  	if( isFull() ) { 
		printf("queue is full\n");
	} 
	rear = (rear+1)%MAX_QUEUE_SIZE;
	queue[rear] = item;
}

QueueObject deleteq() 
{ 
   	if( isEmpty() ) {
		printf("queue is empty\n");
	}
	front = (front+1)%MAX_QUEUE_SIZE;
	return queue[front];
} 

void main()
{
	printf("front=%d rear=%d\n",front, rear);
	addq(1);
	addq(2);
	addq(3);
	printf("deleteq()=%d\n",deleteq());
	printf("deleteq()=%d\n",deleteq());
	printf("deleteq()=%d\n",deleteq());
	printf("front=%d rear=%d\n",front, rear);
}

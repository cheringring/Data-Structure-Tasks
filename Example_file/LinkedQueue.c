/*
 *	LinkedQueue.c
 */

//출처: 천인국 공용해 하상호

#include <stdio.h>
#include <malloc.h>　 

typedef int QueueObject;
typedef struct QueueRec { 
	QueueObject item; 
	struct QueueRec *link; 
} QueueNode;
QueueNode *front, *rear;

void initialize()
{
	front = rear = 0;
}

int isEmpty()
{
	return (front==NULL);
}

int isFull()
{
 	QueueNode *temp = (QueueNode *)malloc(sizeof(QueueNode)); 
	if(temp == NULL ){
		free(temp);
		return 1;
	}
	else return 0;
}

void addq(QueueObject item)
{ 
 	QueueNode *temp = (QueueNode *)malloc(sizeof(QueueNode)); 
	if(temp == NULL ){
		printf("메모리를 할당할 수 없습니다.\n");
	}
	else {
		temp->item = item; 
		temp->link = NULL; 
		if( front ) rear->link = temp;	// 큐가 있을때 
		else front = temp;		// 큐가 공백일때 
		rear = temp; 
	}
}

QueueObject deleteq() 
{ 
 	QueueNode *temp = front;
 	QueueObject item; 
	if( isEmpty() ){
		printf("큐가 비어 있읍니다.\n");
	}
	else {
		item = temp->item; 
		front = front->link; 
		free(temp); 
		return item; 
	}
} 

void main()
{
initialize();
	addq(1);
	addq(2);
	addq(3);
	printf("deleteq()=%d\n",deleteq());
	printf("deleteq()=%d\n",deleteq());
	printf("deleteq()=%d\n",deleteq());
}

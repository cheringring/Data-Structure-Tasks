/*
 *	dynastack.c
 */

//출처: 천인국 공용해 하상호


#include <stdio.h>　 
#include <malloc.h>　 

typedef int StackObject;
struct StackRec { 
	StackObject item;
	struct StackRec *link;
};
struct StackRec *top;

void initialize()
{
	top = NULL;
}

int isEmpty()
{
	return (top == NULL);
}

int isFull()
{
	return 0;
}

void push(StackObject item)
{ 
	struct StackRec *temp = (struct StackRec *)malloc(sizeof(struct StackRec));
	if( temp == NULL ){
		printf("stack is full\n");
	}
	else{
		temp->item = item;
		temp->link = top;
		top = temp;
	}
	
}

StackObject pop() 
{ 
   	if( isEmpty() ) {
		printf("stack is empty\n");
	}
	else{
		struct StackRec *temp=top;
		int item = temp->item; 
		top = top->link;
		free(temp); 
		return item;
	}
} 

void main()
{
initialize();
	push(1);
	push(2);
	push(3);
	printf("%d\n", pop());
	printf("%d\n", pop());
	printf("%d\n", pop());
	printf("%d\n", isEmpty());
}


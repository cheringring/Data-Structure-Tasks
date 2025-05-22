
//출처: 천인국 공용해 하상호

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

typedef int element;
typedef struct DlistNode {
	element data;
	struct DlistNode *llink;
	struct DlistNode *rlink;
} DlistNode;

// 이중 연결 리스트를 초기화
void init(DlistNode *phead) 
{
	phead->llink = phead;
	phead->rlink = phead;
}

// 이중 연결 리스트의 노드를 출력
void display(DlistNode *phead) 
{
	DlistNode *p;
	for(p=phead->rlink; p != phead; p = p->rlink){
		printf("<--- | %x | %d | %x | ---->\n", p->llink, p->data, p->rlink);
	}
	printf("\n");
}
// 노드 new_node를 노드 before의 오른쪽에 삽입한다.
void dinsert_node(DlistNode *before,	DlistNode *new_node) 
{ 
	new_node->llink = before; 
	new_node->rlink = before->rlink; 
	before->rlink->llink = new_node; 
	before->rlink = new_node; 
} 
// 노드 removed를 삭제한다.
void dremove_node(DlistNode *phead_node, 
				DlistNode *removed) 
{ 
	if( removed == phead_node ) return; 
	removed->llink->rlink = removed->rlink; 
	removed->rlink->llink = removed->llink; 
	free(removed); 
} 
// 이중 연결 리스트 테스트 프로그램
void main()
{
	DlistNode head_node;
	DlistNode *p[10];
	int i;
	
	init(&head_node);
	for(i=0;i<5;i++){
		p[i] = (DlistNode *)malloc(sizeof(DlistNode));
		p[i]->data = i;
		// 헤드 노드의 오른쪽에 삽입
		dinsert_node(&head_node, p[i]);
	}
	dremove_node(&head_node, p[4]);
	display(&head_node);
}



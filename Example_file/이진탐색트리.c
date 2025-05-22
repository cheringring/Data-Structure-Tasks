#include <stdio.h>
#include <stdlib.h>

typedef struct _node{
	int data;
	struct _node *leftChild;
	struct _node *rightChild;
}node;

void printInorder(node *ptr);
void printPreorder(node *ptr);
void printPostorder(node *ptr);
void insertNode(node *tree, node *child);
node* creatNode(int num);
bool searchNode(node *tree, int num);
node* searchMinNode(node *tree);
node* searchMaxNode(node *tree);
node* removeNode(node *tree, node *parent, int num);

void main() {
	int menuNum=0, quitFlag=0, creatFlag=1;
	node *Tree;

	printf("메뉴 :1 - 추가, 2 - 삭제, 3 - 출력, 4 - 종료\n");

	while(1)
	{
		int num;
		printf("메뉴 : ");
		scanf_s("%d",&menuNum);

		switch(menuNum) {
		case 1: //추가
			printf("추가 : ");
			scanf_s("%d",&num);
			if(creatFlag==1) {
				Tree=creatNode(num);
				creatFlag=0;
			}
			else {
				if(searchNode(Tree, num))
					printf("에러 : 중복된 숫자입니다 : %d\n", num);
				else
					insertNode(Tree,creatNode(num));
			}
			break;
		case 2: //삭제
			printf("삭제 : ");
			scanf_s("%d", &num);
			if(searchNode(Tree,num)) {
				node *Node = removeNode(Tree, NULL, num);
				free(Node);
			}
			else
				printf("에러 : 존재하지 않는 숫자 입니다 : %d\n", num);
			break;
		case 3:  //출력
			printf("Preorder : ");
			printPreorder(Tree);
			printf("\n");

			printf("Inorder : ");
			printInorder(Tree);
			printf("\n");

			printf("Postorder : ");
			printPostorder(Tree);
			printf("\n");
			break;
		case 4:  //종료
			quitFlag=1;
			break;
		default:
			break;
		}

		if(quitFlag==1)
			break;
	}
}

void printInorder(node *ptr) {
	if(ptr != NULL)
	{
		printInorder(ptr->leftChild);
		printf("%d ",ptr->data);
		printInorder(ptr->rightChild);
	}
	else 
		return;
}

void printPreorder(node *ptr) {
	if(ptr!= NULL) {
		printf("%d ",ptr->data);
		printPreorder(ptr->leftChild);
		printPreorder(ptr->rightChild);
	}
	else
		return;
}

void printPostorder(node *ptr) {
	if(ptr != NULL) {
		printPostorder(ptr->leftChild);
		printPostorder(ptr->rightChild);
		printf("%d ", ptr->data);
	}
	else 
		return;
}

node* creatNode(int num) {
	node *newNode=(node*)malloc(sizeof(node));
	newNode->leftChild=NULL;
	newNode->rightChild=NULL;
	newNode->data=num;
	return newNode;
}

void insertNode(node *tree, node *child) {
	if(tree->data < child->data) {
		if(tree->rightChild==NULL)
			tree->rightChild=child;
		else 
			insertNode(tree->rightChild, child);
	}
	else if(tree->data > child->data) {
		if(tree->leftChild==NULL)
			tree->leftChild=child;
		else
			insertNode(tree->leftChild,child);
	}
}

bool searchNode(node *tree, int num)
{
	if(tree==NULL)
		return false;
	if(tree->data < num)
		return searchNode(tree->rightChild,num);
	else if(tree->data > num)
		return searchNode(tree->leftChild, num);
	else
		return true;
}

node* searchMinNode(node *tree) {
	if(tree==NULL)
		return NULL;
	if(tree->leftChild==NULL)
		return tree;
	else 
		return searchMinNode(tree->leftChild);
}

node* searchMaxNode(node *tree){
	if(tree==NULL)
		return NULL;
	if(tree->rightChild==NULL)
		return tree;
	else 
		return searchMinNode(tree->rightChild);
}

node* removeNode(node *tree, node *parent, int num) {
	node * removedNode = NULL;
	if(tree == NULL)
		return NULL; 
	if(tree->data > num)  // 값을 비교해가면서 목표값 검색
 		removedNode= removeNode(tree->leftChild,tree,num);
	else if(tree->data < num)
		removedNode = removeNode(tree->rightChild, tree, num);
	else { //목표값을 찾은 경우
		removedNode = tree;
		if((tree->leftChild==NULL)&&(tree->rightChild==NULL)) { // 삭제할 노드의 자식 노드들이 없을 경우 (잎노드)
			if(parent->leftChild==tree)
				parent->leftChild=NULL;
			else
				parent->rightChild=NULL;
		}
		else { //삭제할 노드가 자식 노드를 가지고 있을 경우
			if((tree->leftChild != NULL)&&(tree->rightChild!=NULL)) { // 자식노드가 양쪽 다 있는 경우
				node * minNode = searchMaxNode(tree->leftChild); // 1. 삭제할 노드의 왼쪽 트리에서 가장 큰 값을 현재 노드에 위치시킨다.
				// 2. 삭제할 노드의 오른쪽 트리에서 가장 작은 값을 현재 노드에 위치시킨다.
				minNode=removeNode(tree, NULL, minNode->data);
				if(parent->leftChild==tree) {
					parent->leftChild = minNode;
					minNode->rightChild = tree->rightChild;
				}
				else if(parent->rightChild==tree) {
					parent->rightChild = minNode;
					minNode->rightChild=tree->rightChild;
				}
			}
			else // 자식노드가 한쪽에만 있는 경우
			{
				node * temp =NULL;
				if(tree->leftChild!=NULL)
					temp=tree->leftChild;
				else
					temp=tree->rightChild;

				if(parent->leftChild == tree)
					parent->leftChild=temp;
				else
					parent->rightChild=temp;
			}
		}
	}

	return removedNode;
}

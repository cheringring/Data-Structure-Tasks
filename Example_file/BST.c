//출처: 천인국 공용해 하상호


// 이진 탐색 트리를 사용한 영어 사전

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

// 노드의 구조
typedef struct TreeNode {
	int key;
	struct TreeNode *left, *right;
} TreeNode;

// 이진 탐색 트리 출력 함수
void display(TreeNode *p)
{
	if( p != NULL ) {
		printf("(");
		display(p->left);
		printf("%d", p->key);
		display(p->right);
		printf(")");
	}
}
// 반복적인 탐색 함수
TreeNode *search(TreeNode *node, int key) 
{    
	while(node != NULL){ 
	    if( key == node->key ) return node; 
	    else if( key < node->key ) 
		node = node->left; 
	    else 
		node = node->right; 
    } 
    return NULL; 	// 탐색에 실패했을 경우 NULL 반환
} 
// key를 이진 탐색 트리 root에 삽입한다. 
// key가 이미 root안에 있으면 삽입되지 않는다.
void insert_node(TreeNode **root, int key) 
{
    TreeNode *p, *q; // p는 현재 노드, q는 부모 노드 
	TreeNode *n;	 // n은 새로운 노드

	p = *root;
	q = NULL;
	// 탐색을 먼저 수행 
	while (p != NULL){
	  if( key == p->key) return;
	  q = p;
      if( key < p->key ) p = p->left;
	  else p = p->right;
    }
	// item이 트리 안에 없으므로 삽입 가능
	n = (TreeNode *) malloc(sizeof(TreeNode));
	if( n == NULL ) return;
	// 데이터 복사
	n->key = key;
	n->left = n->right = NULL;
	// 부모 노드와 링크 연결
	if( q != NULL ) 
		if( key < q->key ) 
			q->left = n;
		else q->right = n;
	else *root = n;
}
void delete_node(TreeNode *node, int key)
{
TreeNode *p, *child, *succ, *succ_p, *t;

// key를 갖는 노드 t를 탐색, p는 t의 부모노드
p = NULL;
t = node;
while( t != NULL && t->key != key ){
	p = t;
	t = ( key < t->key ) ? t->left : t->right;
}
if( t == NULL ) { 	// 탐색트리에 없는 키
	printf("key is not in the tree");
	return;
}
// 단말노드인 경우
if( (t->left==NULL) && (t->right==NULL) ){ 
	if( p->left == t ) 
		p->left = NULL;
	else   p->right = NULL;
}
// 하나의 자식만 가지는 경우
else if((t->left==NULL)||(t->right==NULL)){
	child = (t->left != NULL) ? t->left : t->right;
	if( p->left == t ) 
		p->left = child;
	else p->right = child;
}
else{		// 두개의 자식을 가지는 경우
	succ = t->right;
	while(succ->left != NULL){
		succ_p = succ;
		succ = succ->left;
	}
	if( succ_p->left == succ )
		succ_p->left = succ->right;
	else 
		succ_p->right = succ->right;
	t->key = succ->key;
	t = succ;
}
free(t);
}
//
int get_maximum(TreeNode *root)
{
	while(root->right != NULL) root=root->right;
	return root->key;
}
//
int get_minimum(TreeNode *root)
{
	while(root->left != NULL) root=root->left;
	return root->key;
}
//
int get_leaf_count(TreeNode *node)
{
	int count=0;
	if( node != NULL ){
		if( node->left==NULL && node->right==NULL ) return 1;
        else count = get_leaf_count(node->left)+ 
			get_leaf_count(node->right);
	}
    return count;
}
//
int get_node_count(TreeNode *node)
{
	int count=0;
	if( node != NULL )
        count = 1 + get_node_count(node->left)+ 
			get_node_count(node->right);
    return count;
}
//
void delete_all(TreeNode *root)
{
	if( root!= NULL ){
		delete_all(root->left);
		delete_all(root->right);
		free(root);
	}
}
//
int get_height(TreeNode *node)
{
	int height=0;
	if( node != NULL )
        height = 1 + max(get_height(node->left), 
			get_height(node->right));
    return height;
}
//
void help()
{
   printf("**************\n");
   printf("i: 입력\n");
   printf("d: 삭제\n");
   printf("s: 탐색\n");
/*   printf("x: 전체 노드 삭제\n");
   printf("h: 높이\n");
   printf("c: 노드의 개수\n");
   printf("m: 최대값\n");
   printf("n: 최소값\n");
   printf("t: 단말노드의 개수\n"); */
   printf("p: 출력\n");
   printf("q: 종료\n");
   printf("**************\n");
}

// 이진 탐색 트리 테스트 프로그램 
void main()
{
    char command;
 	int key;
	TreeNode *root=NULL;
	TreeNode *tmp;
	int count;

    do{
		help();
       	command = getchar();
   
		switch(command){
		case 'i':
			printf("숫자:");
			scanf("%d", &key);
			insert_node(&root, key);
			break;
		case 'd':
			printf("숫자:");
			scanf("%d", &key);
			delete_node(root, key);
			break;
		case 'm':
			printf("%d\n",get_maximum(root));
			break;
		case 'n':
			printf("%d\n",get_minimum(root));
			break;
		case 't':
			count=0;
			count = get_leaf_count(root);
			printf("%d\n",count);
			break;
		case 'c':
			count=0;
			count = get_node_count(root);
			printf("%d\n",count);
			break;
		case 'x':
			delete_all(root);
			root=NULL;
			break;
		case 'h':
			printf("%d\n",get_height(root));
			break;
		case 'p':
			display(root);
			printf("\n");
			break;
		case 's':
			printf("숫자:");
			scanf("%d", &key);
			tmp=search(root, key);
			if( tmp != NULL )
				printf("탐색성공:%d\n", tmp->key);
			break;
		}
		fflush(stdin);
		
    } while(command != 'q');
}

//출처: 천인국 공용해 하상호



#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct tree_node {
	int data;
	struct tree_node *left_child, *right_child;
} tnode_str;

tnode_str *root;

inorder( tnode_str *ptr ){
    if ( ptr ){
	 inorder( ptr->left_child );       // 좌측 서브 트리 검사
	 printf("%d ", ptr->data );    	// 노드 방문
	 inorder( ptr->right_child );      // 우측 서브 트리 검사
    }
}
preorder( tnode_str *ptr ){
    if ( ptr ){
        printf("%d ", ptr->data );        // 노드 방문
	  preorder( ptr->left_child );      // 좌측 서브 트리 검사
	  preorder( ptr->right_child );     // 우측 서브 트리 검사
    }
}
postorder( tnode_str *ptr ){
    if ( ptr ){
	 postorder( ptr->left_child );       // 좌측 서브 트리 검사
	 postorder( ptr->right_child );     // 우측 서브 트리 검사
	 printf("%d ", ptr->data );     	   // 노드 방문
    }
}

//
//   		    n1
//		   / |
//		  n2  n3
//
void main()
{
	tnode_str *n1, *n2, *n3;

	n1 = (tnode_str *)malloc(sizeof(tnode_str));
	n2 = (tnode_str *)malloc(sizeof(tnode_str));
	n3 = (tnode_str *)malloc(sizeof(tnode_str));

	// 첫번쨰 노드를 설정한다.
	n1->data = 10;
	n1->left_child = n2;
	n1->right_child = n3;
	
	// 두번쨰 노드를 설정한다.
	n2->data = 20;
	n2->left_child = NULL;
	n2->right_child = NULL;

	// 세번쨰 노드를 설정한다.
	n3->data = 30;
	n3->left_child = NULL;
	n3->right_child = NULL;

	// 루트 노드를 n1으로 한다.
	root = n1;
	printf("\ninorder\n");
	inorder(root);
	printf("\npreorder\n");
	preorder(root);
	printf("\npostorder\n");
	postorder(root);
}


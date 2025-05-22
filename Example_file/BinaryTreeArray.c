//출처: 천인국 공용해 하상호


#include <stdio.h>

#define MAX_SIZE 1000
char tree_array[MAX_SIZE];

void make_tree()
{
	tree_array[1] = 'A';
	tree_array[2] = 'B';
	tree_array[3] = 'C';
	tree_array[4] = 'D';
	tree_array[5] = 'E';
	tree_array[6] = 'F';
}

void inorder( int index )
{
	printf("inorder(%d)\n",index);
	if( tree_array[index] == 0 ){
		return;
	}
	inorder( 2*index );       // 좌측 서브 트리 검사
	printf("<%c>", tree_array[index] );    	// 노드 방문
	inorder( 2*index+1 );      // 우측 서브 트리 검사
}

void main()
{
	make_tree();
	inorder(1);
}

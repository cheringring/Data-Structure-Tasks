//출처: 천인국 공용국 하상호

// graph_def.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#define MAX_VERTICES 4
int adj_mat[MAX_VERTICES][MAX_VERTICES]={
	{0,1,0,1},
	{1,0,1,1},
	{0,1,0,1},
	{1,1,1,0} };
int visited[MAX_VERTICES];
int n=4;		// 현재 노드의 개수

// 인접배열로 표현된 그래프에 대한 깊이우선탐색
void graph_dfs_mat(int v)
{
   int w;
   visited[v] = 1;
   printf("%d ", v);
   for(w=0; w<n; w++) 
 	 if( adj_mat[v][w] && !visited[w] )
	   	graph_dfs_mat(w);
}
void main()
{
	graph_dfs_mat(0);
}
/*  */
void read_graph(char *file_name)
{
	int i,j;
	FILE *fp = fopen(file_name, "r");
	if( fp == NULL ) return;
	fscanf(fp, "%d\n", &n);
	for(i=0;i<n;i++){
	for(j=0;j<n;j++){
		fscanf(fp, "%d", &adj_mat[i][j]);
	}
	}
	fclose(fp);
}

/*  */
void write_graph(char *file_name)
{
	int i,j;
	FILE *fp = fopen(file_name, "w");
	if( fp == NULL ) return;
	fprintf(fp, "%d\n", n);
	for(i=0;i<n;i++){
	for(j=0;j<n;j++){
		fprintf(fp, " %d ", adj_mat[i][j]);
	}
	fprintf(fp, "\n");
	}
	fclose(fp);
}

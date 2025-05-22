//출처: 천인국 공용국 하상호

#include <stdio.h>
#include <limits.h>

#define TRUE	1
#define FALSE	0
#define NODES	7		/* 노드의 수 */
#define INF 	9999	/* 무한 값(연결이 없는 경우) */

/* 네트워크의 인접행렬 */
int cost[NODES][NODES]={
	{	0,	7,	INF, INF,	3,	10, INF },
	{	7,	0,	  4,  10,	2,	 6, INF },
	{ INF,	4,	  0,   2, INF, INF, INF },
	{ INF, 10,	  2,   0,  11,	 9,   4 },
	{	3,	2,	INF,  11,	0, INF,   5 },
	{  10,	6,	INF,   9, INF,	 0, INF },
	{ INF, INF, INF,   4,	5, INF,   0 }};

int distance[NODES];/* 시작노드로부터의 최단경로 거리 */
int path[NODES];	/* 경유 노드  */
int found[NODES];		/* 방문한 노드 표시 */

void print_distance()
{
	int i;
	for(i=0;i<NODES;i++){
		printf("%d ", distance[i]);
	}
	printf("\n");
}

int choose(int distance[], int n, int found[])
{
	int i, min, minpos;
	min = INT_MAX;
	minpos = -1;
	for(i=0;i<n;i++)
		if( distance[i]< min && ! found[i] ){
			min = distance[i];
			minpos=i;
		}
	return minpos;
}

void shortest_path(int start, int cost[][NODES],
				   int distance[], int n, int found[]) /* 시작노드 start  */
{  
   int i, u, w;
   for(i=0; i<n; i++) /* 초기화 */
   {
	  distance[i] = cost[start][i];
	  path[i] = start;
	  found[i] = FALSE;
   }
   found[start] = TRUE;    /* 시작노드 방문 표시 */
   distance[start] = 0;
   for(i=0; i<n; i++){
	   print_distance();
	  u = choose(distance, n, found);
	  found[u] = TRUE;
	  for(w=0;w<n; w++) 
		   if(!found[w])
			   if( distance[u]+cost[u][w] < distance[w] ) {
				   distance[w] = distance[u]+cost[u][w];	
				path[w] = u;		/* 경유노드 저장 */
		   }
   }
}

void print_path(int start, int end)
{  
	if( path[end] != start)
		print_path(start, path[end]);
	printf("<%d->%d> ", path[end], end);
}


void main()
{
	shortest_path(0, cost, distance, NODES, found);
	print_path(0,3);
	print_distance();
}


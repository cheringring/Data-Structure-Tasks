#include <stdio.h>
#include <malloc.h>
#include <conio.h>

#define FALSE 0
#define TRUE 1
#define START 0
#define SIZE 8
#define Null 0


int init_list();
int dfs(int v);

struct node{
 int vertex;
 struct node *link;
};

struct node *graph[SIZE];
int visited[SIZE];

void main()
{
 int i;
 for(i=0; i<=7; i++)
  visited[i] = FALSE;
 //clrscr();
 init_list();
 dfs(START);
}

int init_list()
{
 struct node *new_node;
 int i;

 for(i=0; i<SIZE; i++)
  graph[i] = Null;
 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[0];
 graph[0] = new_node;
 new_node ->vertex = 2;
 new_node = (struct node*)malloc(sizeof(new_node));
 new_node ->link = graph[0];
 graph[0] = new_node;
 new_node ->vertex = 1;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[1];
 graph[1] = new_node;
 new_node ->vertex = 4;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[1];
 graph[1] = new_node;
 new_node ->vertex = 3;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[1];
 graph[1] = new_node;
 new_node ->vertex = 0;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[2];
 graph[2] = new_node;
 new_node ->vertex = 6;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[2];
 graph[2] = new_node;
 new_node ->vertex = 5;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[2];
 graph[2] = new_node;
 new_node ->vertex = 0;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[3];
 graph[3] = new_node;
 new_node ->vertex = 7;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[3];
 graph[3] = new_node;
 new_node ->vertex = 1;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[4];
 graph[4] = new_node;
 new_node ->vertex = 7;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[4];
 graph[4] = new_node;
 new_node ->vertex = 1;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[5];
 graph[5] = new_node;
 new_node ->vertex = 7;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[5];
 graph[5] = new_node;
 new_node ->vertex = 2;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[6];
 graph[6] = new_node;
 new_node ->vertex = 7;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[6];
 graph[6] = new_node;
 new_node ->vertex = 2;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[7];
 graph[7] = new_node;
 new_node ->vertex = 5;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[7];
 graph[7] = new_node;
 new_node ->vertex = 4;

 new_node = (struct node*)malloc(sizeof(new_node));
 new_node -> link = graph[7];
 graph[7] = new_node;
 new_node ->vertex = 3;
 return 0;
}

int dfs(int v)
{
 int w;
 visited[v] = TRUE;

 printf("Visited vertex -> %5d\n", v);
 do {
  w = graph[v] ->vertex;
  graph[v] = graph[v]->link;
  if(!visited[w]) dfs(w);
 }while(graph[v] != Null);
 return 0;

}


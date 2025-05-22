#include <conio.h>
#include <stdio.h>
int main(void)
{
 int num[3];
 int i,j,c=1;
 printf("3개의 숫자를 입력하세요\n형식 : 10 3 9 \nENTER : ");
 scanf("%d %d %d", &num[0], &num[1], &num[2]);
 printf("배열정렬 입력 :%3d %3d %3d\n",num[0], num[1], num[2]);
 // 선택 정렬 프로그램 오름차순 정렬
 for(i=0; i < 3; i++)
 {
  for(j=i+1; j<3; j++)
  {
   if( num[j] < num[i] )
   {
    int temp;
    temp = num[j];
    num[j] = num[i];
    num[i] = temp;
   }
  }
 }
 // 선택 정렬 프로그램 오름차순 정렬 출력
 printf("오름차순 정렬 :");
 for(i=0; i < 3; i++)
  printf("%3d ",num[i]);
 printf("\n");
 // 선택 정렬 프로그램 내림차순 정렬 출력
 printf("내림차순 정렬 :");
 for(i=3; i > 0; i--)
  printf("%3d ",num[i-1]);
}

#include <stdio.h>

int BinarySearch(int*,int,int);
int main(void) {
	int arr[]={1,2,3,5,9,12,25,31,45};
	int idx;

	idx= BinarySearch(arr, sizeof(arr)/sizeof(int),3);
	if(idx== -1)
		printf("검색할 대상없음\n");
	else
		printf("검색한 인덱스 : %d \n",idx);
	return 0;
}

int BinarySearch(int ar[], int len, int target)
{
	int first =0; // 탐색 대상의 시작 인덱스 값
	int last = len-1; // 탐색 대상의 마지막 인덱스 값
	int mid;

	while(first<=last) {
		mid=(first+last)/2; // 탐색 대상의 중앙을 찾음
		if(target==ar[mid]) // 중앙에 저장된 것이 타겟이면
			return mid; // 탐색 완료
		else // 타겟이 아니라면 탐색 대상을 반으로 줄인다
		{
			if(target < ar[mid])
				last=mid-1;
			else
				first=mid+1;
		}
	}
	return -1; // 타겟이 없을 때 반환되는 값 -1
}

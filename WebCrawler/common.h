#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0
#define MAX_URL_LENGTH 2048
#define MAX_THREADS 10
#define MAX_VERTICES 100  // 최대 정점(URL) 수
#define HASH_SIZE 10007  // 소수를 사용하여 해시 충돌 최소화

// 에러 처리 함수 선언
extern void error(const char *message);

#endif /* COMMON_H */

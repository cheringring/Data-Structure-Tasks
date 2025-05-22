#ifndef URL_SET_H
#define URL_SET_H

#include "common.h"

// 해시 테이블 노드 구조체
typedef struct HashNode {
    char url[MAX_URL_LENGTH];
    struct HashNode *next;
} HashNode;

// 해시 테이블 구조체
typedef struct {
    HashNode *table[HASH_SIZE];
    pthread_mutex_t mutex;  // 멀티스레딩 환경에서 동기화를 위한 뮤텍스
    int count;              // 저장된 URL 수
} URLSet;

// 해시 테이블 초기화
void url_set_init(URLSet *set);

// URL의 해시값 계산
unsigned int hash_function(const char *url);

// URL이 해시 테이블에 있는지 확인
int url_set_contains(URLSet *set, const char *url);

// URL을 해시 테이블에 추가
int url_set_add(URLSet *set, const char *url);

// 해시 테이블 정리 (메모리 해제)
void url_set_cleanup(URLSet *set);

#endif /* URL_SET_H */

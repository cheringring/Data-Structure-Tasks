#include "url_set.h"

// 해시 테이블 초기화 함수
void url_set_init(URLSet *set) {
    int i;
    for (i = 0; i < HASH_SIZE; i++) {
        set->table[i] = NULL;
    }
    set->count = 0;
    pthread_mutex_init(&set->mutex, NULL);
}

// URL의 해시값 계산 함수 (djb2 알고리즘)
unsigned int hash_function(const char *url) {
    unsigned int hash = 5381;
    int c;
    
    while ((c = *url++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash % HASH_SIZE;
}

// URL이 해시 테이블에 있는지 확인하는 함수
int url_set_contains(URLSet *set, const char *url) {
    // 스레드 안전성을 위한 뮤텍스 잠금
    pthread_mutex_lock(&set->mutex);
    
    // 해시값 계산
    unsigned int hash = hash_function(url);
    
    // 해당 버킷에서 URL 검색
    HashNode *current = set->table[hash];
    while (current != NULL) {
        if (strcmp(current->url, url) == 0) {
            // URL 발견
            pthread_mutex_unlock(&set->mutex);
            return TRUE;
        }
        current = current->next;
    }
    
    // URL을 찾지 못함
    pthread_mutex_unlock(&set->mutex);
    return FALSE;
}

// URL을 해시 테이블에 추가하는 함수
int url_set_add(URLSet *set, const char *url) {
    // 스레드 안전성을 위한 뮤텍스 잠금
    pthread_mutex_lock(&set->mutex);
    
    // 해시값 계산
    unsigned int hash = hash_function(url);
    
    // 이미 URL이 존재하는지 확인 (뮤텍스 잠금 상태에서)
    HashNode *current = set->table[hash];
    while (current != NULL) {
        if (strcmp(current->url, url) == 0) {
            // URL 발견 - 이미 존재함
            pthread_mutex_unlock(&set->mutex);
            printf("URL 이미 존재: %s\n", url);
            return FALSE;
        }
        current = current->next;
    }
    
    printf("URL 새로 추가: %s\n", url);
    
    // 새 노드 생성
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    if (new_node == NULL) {
        error("해시 노드 메모리 할당 실패");
    }
    
    // URL 복사
    strncpy(new_node->url, url, MAX_URL_LENGTH - 1);
    new_node->url[MAX_URL_LENGTH - 1] = '\0';  // 문자열 끝 보장
    
    // 해시 테이블에 노드 추가 (체이닝 방식)
    new_node->next = set->table[hash];
    set->table[hash] = new_node;
    
    set->count++;
    
    // 뮤텍스 잠금 해제
    pthread_mutex_unlock(&set->mutex);
    
    return TRUE;
}

// 해시 테이블 정리 함수 (메모리 해제)
void url_set_cleanup(URLSet *set) {
    // 스레드 안전성을 위한 뮤텍스 잠금
    pthread_mutex_lock(&set->mutex);
    
    // 모든 버킷 순회
    int i;
    for (i = 0; i < HASH_SIZE; i++) {
        // 각 버킷의 연결 리스트 순회
        HashNode *current = set->table[i];
        HashNode *next;
        
        while (current != NULL) {
            next = current->next;
            free(current);
            current = next;
        }
        
        set->table[i] = NULL;
    }
    
    set->count = 0;
    
    // 뮤텍스 잠금 해제
    pthread_mutex_unlock(&set->mutex);
    
    // 뮤텍스 파괴
    pthread_mutex_destroy(&set->mutex);
}

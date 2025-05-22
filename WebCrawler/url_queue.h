#ifndef URL_QUEUE_H
#define URL_QUEUE_H

#include "common.h"

// URL 정보를 저장하는 구조체
typedef struct {
    char url[MAX_URL_LENGTH];
    int depth;  // 크롤링 깊이
} URLItem;

// 큐 노드 구조체
typedef struct QueueNode {
    URLItem item;
    struct QueueNode *link;
} QueueNode;

// 큐 구조체
typedef struct {
    QueueNode *front;
    QueueNode *rear;
    pthread_mutex_t mutex;  // 멀티스레딩 환경에서 동기화를 위한 뮤텍스
    int size;               // 현재 큐에 있는 항목 수
} URLQueue;

// 큐 초기화
void url_queue_init(URLQueue *queue);

// 큐가 비어있는지 확인
int url_queue_is_empty(URLQueue *queue);

// 큐에 URL 추가
void url_queue_enqueue(URLQueue *queue, const char *url, int depth);

// 큐에서 URL 제거 및 반환
int url_queue_dequeue(URLQueue *queue, URLItem *item);

// 큐 정리 (메모리 해제)
void url_queue_cleanup(URLQueue *queue);

#endif /* URL_QUEUE_H */

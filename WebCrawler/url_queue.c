#include "url_queue.h"

// 큐 초기화 함수
void url_queue_init(URLQueue *queue) {
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    pthread_mutex_init(&queue->mutex, NULL);
}

// 큐가 비어있는지 확인하는 함수
int url_queue_is_empty(URLQueue *queue) {
    return (queue->front == NULL);
}

// 큐에 URL 추가하는 함수
void url_queue_enqueue(URLQueue *queue, const char *url, int depth) {
    printf("큐 추가 시도: %s (깊이: %d)\n", url, depth);
    
    // 스레드 안전성을 위한 뮤텍스 잠금
    pthread_mutex_lock(&queue->mutex);
    
    // 새 노드 생성
    QueueNode *new_node = (QueueNode *)malloc(sizeof(QueueNode));
    if (new_node == NULL) {
        error("큐 노드 메모리 할당 실패");
    }
    
    // URL 복사 및 깊이 설정
    strncpy(new_node->item.url, url, MAX_URL_LENGTH - 1);
    new_node->item.url[MAX_URL_LENGTH - 1] = '\0';  // 문자열 끝 보장
    new_node->item.depth = depth;
    new_node->link = NULL;
    
    // 큐에 노드 추가
    if (queue->rear == NULL) {  // 큐가 비어있는 경우
        queue->front = new_node;
        queue->rear = new_node;
    } else {  // 큐에 요소가 있는 경우
        queue->rear->link = new_node;
        queue->rear = new_node;
    }
    
    queue->size++;
    
    // 뮤텍스 잠금 해제
    pthread_mutex_unlock(&queue->mutex);
}

// 큐에서 URL 제거 및 반환하는 함수
int url_queue_dequeue(URLQueue *queue, URLItem *item) {
    // 스레드 안전성을 위한 뮤텍스 잠금
    pthread_mutex_lock(&queue->mutex);
    
    // 큐가 비어있는 경우
    if (url_queue_is_empty(queue)) {
        printf("큐가 비어있음 - dequeue 실패\n");
        pthread_mutex_unlock(&queue->mutex);
        return FALSE;
    }
    
    printf("큐에서 URL 꼬냄 (현재 큐 크기: %d)\n", queue->size);
    
    // 첫 번째 노드 제거
    QueueNode *temp = queue->front;
    
    // 항목 복사
    *item = temp->item;
    
    // 큐 업데이트
    queue->front = temp->link;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    
    // 메모리 해제
    free(temp);
    
    queue->size--;
    
    // 뮤텍스 잠금 해제
    pthread_mutex_unlock(&queue->mutex);
    
    return TRUE;
}

// 큐 정리 함수 (메모리 해제)
void url_queue_cleanup(URLQueue *queue) {
    // 스레드 안전성을 위한 뮤텍스 잠금
    pthread_mutex_lock(&queue->mutex);
    
    // 모든 노드 제거
    QueueNode *current = queue->front;
    QueueNode *next;
    
    while (current != NULL) {
        next = current->link;
        free(current);
        current = next;
    }
    
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    
    // 뮤텍스 잠금 해제
    pthread_mutex_unlock(&queue->mutex);
    
    // 뮤텍스 파괴
    pthread_mutex_destroy(&queue->mutex);
}

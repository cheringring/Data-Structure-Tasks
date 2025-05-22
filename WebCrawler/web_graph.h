#ifndef WEB_GRAPH_H
#define WEB_GRAPH_H

#include "common.h"

// 그래프 노드 구조체 (인접 리스트 표현)
typedef struct GraphNode {
    char url[MAX_URL_LENGTH];
    struct GraphNode *next;
} GraphNode;

// 그래프 구조체
typedef struct {
    int max_vertices;           // 최대 정점 수
    int num_vertices;           // 현재 정점 수
    char **vertices;            // 정점 배열 (URL 저장)
    GraphNode **adj_list;       // 인접 리스트
    pthread_mutex_t mutex;      // 멀티스레딩 환경에서 동기화를 위한 뮤텍스
} WebGraph;

// 그래프 초기화
void web_graph_init(WebGraph *graph, int max_vertices);

// URL을 그래프에 정점으로 추가
int web_graph_add_vertex(WebGraph *graph, const char *url);

// URL 인덱스 찾기
int web_graph_find_vertex(WebGraph *graph, const char *url);

// 두 URL 사이에 간선 추가 (소스 URL에서 타겟 URL로의 링크)
void web_graph_add_edge(WebGraph *graph, const char *source_url, const char *target_url);

// 그래프 정보 출력
void web_graph_print(WebGraph *graph);

// 그래프 정리 (메모리 해제)
void web_graph_cleanup(WebGraph *graph);

// 그래프를 DOT 형식으로 파일에 저장 (시각화용)
void web_graph_save_dot(WebGraph *graph, const char *filename);

#endif /* WEB_GRAPH_H */

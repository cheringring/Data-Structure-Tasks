#include "web_graph.h"

// 그래프 초기화 함수
void web_graph_init(WebGraph *graph, int max_vertices) {
    int i;
    
    graph->max_vertices = max_vertices;
    graph->num_vertices = 0;
    
    // 정점 배열 할당
    graph->vertices = (char **)malloc(max_vertices * sizeof(char *));
    if (graph->vertices == NULL) {
        error("그래프 정점 배열 메모리 할당 실패");
    }
    
    // 각 정점에 대한 메모리 할당
    for (i = 0; i < max_vertices; i++) {
        graph->vertices[i] = (char *)malloc(MAX_URL_LENGTH * sizeof(char));
        if (graph->vertices[i] == NULL) {
            error("그래프 정점 메모리 할당 실패");
        }
    }
    
    // 인접 리스트 배열 할당
    graph->adj_list = (GraphNode **)malloc(max_vertices * sizeof(GraphNode *));
    if (graph->adj_list == NULL) {
        error("그래프 인접 리스트 메모리 할당 실패");
    }
    
    // 인접 리스트 초기화
    for (i = 0; i < max_vertices; i++) {
        graph->adj_list[i] = NULL;
    }
    
    // 뮤텍스 초기화
    pthread_mutex_init(&graph->mutex, NULL);
}

// URL을 그래프에 정점으로 추가하는 함수
int web_graph_add_vertex(WebGraph *graph, const char *url) {
    printf("DEBUG: web_graph_add_vertex 시작 (url: %s)\n", url);
    int index;
    
    // 스레드 안전성을 위한 뮤텍스 잠금
    printf("DEBUG: web_graph_add_vertex - 뮤텍스 잠금 시도\n");
    pthread_mutex_lock(&graph->mutex);
    printf("DEBUG: web_graph_add_vertex - 뮤텍스 잠금 성공\n");
    
    // 이미 존재하는 URL인지 확인
    printf("DEBUG: 이미 존재하는 URL인지 확인 시도\n");
    index = web_graph_find_vertex(graph, url);
    printf("DEBUG: web_graph_find_vertex 결과 - index: %d\n", index);
    if (index != -1) {
        // 이미 존재하는 URL
        printf("DEBUG: 이미 존재하는 URL - index: %d\n", index);
        pthread_mutex_unlock(&graph->mutex);
        return index;
    }
    
    // 최대 정점 수 초과 확인
    printf("DEBUG: 최대 정점 수 초과 확인 (num_vertices: %d, max_vertices: %d)\n", graph->num_vertices, graph->max_vertices);
    if (graph->num_vertices >= graph->max_vertices) {
        fprintf(stderr, "그래프 정점 수 초과\n");
        printf("DEBUG: 그래프 정점 수 초과!\n");
        pthread_mutex_unlock(&graph->mutex);
        return -1;
    }
    
    // 새 정점 추가
    printf("DEBUG: 새 정점 추가 시도 (index: %d, url: %s)\n", graph->num_vertices, url);
    strncpy(graph->vertices[graph->num_vertices], url, MAX_URL_LENGTH - 1);
    graph->vertices[graph->num_vertices][MAX_URL_LENGTH - 1] = '\0';  // 문자열 끝 보장
    printf("DEBUG: 새 정점 추가 성공\n");
    
    // 정점 수 증가
    index = graph->num_vertices;
    graph->num_vertices++;
    printf("DEBUG: 정점 수 증가 후 (num_vertices: %d)\n", graph->num_vertices);
    
    // 뮤텍스 잠금 해제
    pthread_mutex_unlock(&graph->mutex);
    printf("DEBUG: web_graph_add_vertex - 뮤텍스 잠금 해제\n");
    
    printf("DEBUG: web_graph_add_vertex 완료 - index: %d\n", index);
    return index;
}

// URL 인덱스 찾는 함수
int web_graph_find_vertex(WebGraph *graph, const char *url) {
    printf("DEBUG: web_graph_find_vertex 시작 (url: %s)\n", url);
    int i;
    
    printf("DEBUG: 정점 수: %d\n", graph->num_vertices);
    for (i = 0; i < graph->num_vertices; i++) {
        printf("DEBUG: 비교 중 (%d): %s\n", i, graph->vertices[i]);
        if (strcmp(graph->vertices[i], url) == 0) {
            printf("DEBUG: URL 발견 (index: %d)\n", i);
            return i;  // URL 발견
        }
    }
    
    printf("DEBUG: URL을 찾지 못함\n");
    return -1;  // URL을 찾지 못함
}

// 두 URL 사이에 간선 추가하는 함수 (소스 URL에서 타겟 URL로의 링크)
void web_graph_add_edge(WebGraph *graph, const char *source_url, const char *target_url) {
    printf("DEBUG: web_graph_add_edge 시작 (source: %s, target: %s)\n", source_url, target_url);
    
    // 소스 URL 인덱스 찾기 (없으면 추가)
    int source_index;
    int target_index;
    
    // 뮤텍스 없이 정점 존재 여부 확인
    source_index = web_graph_find_vertex(graph, source_url);
    printf("DEBUG: source_index = %d\n", source_index);
    
    // 소스 URL이 존재하지 않으면 추가
    if (source_index == -1) {
        printf("DEBUG: 소스 URL 인덱스 추가 시도\n");
        source_index = web_graph_add_vertex(graph, source_url);
        if (source_index == -1) {
            printf("DEBUG: 소스 URL 인덱스 추가 실패\n");
            return;  // 추가 실패
        }
        printf("DEBUG: 소스 URL 인덱스 추가 성공 (index: %d)\n", source_index);
    }
    
    // 타겟 URL 인덱스 찾기 (없으면 추가)
    target_index = web_graph_find_vertex(graph, target_url);
    printf("DEBUG: target_index = %d\n", target_index);
    
    // 타겟 URL이 존재하지 않으면 추가
    if (target_index == -1) {
        printf("DEBUG: 타겟 URL 인덱스 추가 시도\n");
        target_index = web_graph_add_vertex(graph, target_url);
        if (target_index == -1) {
            printf("DEBUG: 타겟 URL 인덱스 추가 실패\n");
            return;  // 추가 실패
        }
        printf("DEBUG: 타겟 URL 인덱스 추가 성공 (index: %d)\n", target_index);
    }
    
    // 여기에서 뮤텍스 잠금 - 실제 간선 추가 작업에만 뮤텍스 사용
    pthread_mutex_lock(&graph->mutex);
    printf("DEBUG: 뮤텍스 잠금 성공 (간선 추가용)\n");
    
    // 이미 간선이 존재하는지 확인
    printf("DEBUG: 이미 간선이 존재하는지 확인 시작\n");
    GraphNode *current = graph->adj_list[source_index];
    while (current != NULL) {
        if (strcmp(current->url, target_url) == 0) {
            // 이미 간선 존재
            printf("DEBUG: 이미 간선 존재, 추가 중단\n");
            pthread_mutex_unlock(&graph->mutex);
            return;
        }
        current = current->next;
    }
    printf("DEBUG: 새 간선 추가 필요\n");
    
    // 새 간선 추가
    printf("DEBUG: GraphNode 메모리 할당 시도 (sizeof(GraphNode): %zu)\n", sizeof(GraphNode));
    GraphNode *new_node = (GraphNode *)malloc(sizeof(GraphNode));
    if (new_node == NULL) {
        printf("ERROR: 그래프 노드 메모리 할당 실패\n");
        error("그래프 노드 메모리 할당 실패");
    }
    printf("DEBUG: GraphNode 메모리 할당 성공\n");
    
    // 타겟 URL 복사
    printf("DEBUG: 타겟 URL 복사 시도 (target_url: %s, MAX_URL_LENGTH: %d)\n", target_url, MAX_URL_LENGTH);
    strncpy(new_node->url, target_url, MAX_URL_LENGTH - 1);
    new_node->url[MAX_URL_LENGTH - 1] = '\0';  // 문자열 끝 보장
    printf("DEBUG: 타겟 URL 복사 성공\n");
    
    // 인접 리스트에 노드 추가
    printf("DEBUG: 인접 리스트에 노드 추가 시도\n");
    new_node->next = graph->adj_list[source_index];
    graph->adj_list[source_index] = new_node;
    printf("DEBUG: 인접 리스트에 노드 추가 성공\n");
    
    // 뮤텍스 잠금 해제
    pthread_mutex_unlock(&graph->mutex);
    printf("DEBUG: web_graph_add_edge 완료 (source: %s, target: %s)\n", source_url, target_url);
}

// 그래프 정보 출력 함수
void web_graph_print(WebGraph *graph) {
    int i;
    
    printf("웹 그래프 정보 (정점 수: %d)\n", graph->num_vertices);
    
    for (i = 0; i < graph->num_vertices; i++) {
        printf("[%d] %s -> ", i, graph->vertices[i]);
        
        // 인접 리스트 출력
        GraphNode *current = graph->adj_list[i];
        while (current != NULL) {
            printf("%s, ", current->url);
            current = current->next;
        }
        
        printf("\n");
    }
}

// 그래프 정리 함수 (메모리 해제)
void web_graph_cleanup(WebGraph *graph) {
    int i;
    
    // 스레드 안전성을 위한 뮤텍스 잠금
    pthread_mutex_lock(&graph->mutex);
    
    // 인접 리스트 메모리 해제
    for (i = 0; i < graph->max_vertices; i++) {
        GraphNode *current = graph->adj_list[i];
        GraphNode *next;
        
        while (current != NULL) {
            next = current->next;
            free(current);
            current = next;
        }
    }
    
    // 인접 리스트 배열 메모리 해제
    free(graph->adj_list);
    
    // 정점 배열 메모리 해제
    for (i = 0; i < graph->max_vertices; i++) {
        free(graph->vertices[i]);
    }
    free(graph->vertices);
    
    // 뮤텍스 잠금 해제
    pthread_mutex_unlock(&graph->mutex);
    
    // 뮤텍스 파괴
    pthread_mutex_destroy(&graph->mutex);
}

// 그래프를 DOT 형식으로 파일에 저장하는 함수 (시각화용)
void web_graph_save_dot(WebGraph *graph, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "파일 열기 실패: %s\n", filename);
        return;
    }
    
    // DOT 파일 헤더 - 레이아웃 개선
    fprintf(file, "digraph WebGraph {\n");
    fprintf(file, "  rankdir=LR;\n");
    fprintf(file, "  node [shape=box, style=filled, fillcolor=lightblue, fontsize=10, width=0.3, height=0.2];\n");
    fprintf(file, "  edge [arrowsize=0.6, penwidth=0.8];\n");
    fprintf(file, "  graph [overlap=false, splines=true, nodesep=0.3, ranksep=0.5];\n");
    
    // 모든 정점과 간선 출력
    int i;
    for (i = 0; i < graph->num_vertices; i++) {
        // 정점 ID 생성 (URL은 특수문자가 있을 수 있어 인덱스 사용)
        fprintf(file, "  node%d [label=\"%s\"];\n", i, graph->vertices[i]);
        
        // 간선 출력
        GraphNode *current = graph->adj_list[i];
        while (current != NULL) {
            int target_index = web_graph_find_vertex(graph, current->url);
            if (target_index != -1) {
                fprintf(file, "  node%d -> node%d;\n", i, target_index);
            }
            current = current->next;
        }
    }
    
    // DOT 파일 푸터
    fprintf(file, "}\n");
    
    fclose(file);
    printf("그래프를 DOT 파일로 저장: %s\n", filename);
}

#include "crawler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// 크롤러 초기화 함수
void crawler_init(Crawler *crawler, int max_depth, int max_urls, int max_vertices) {
    // URL 큐 초기화
    url_queue_init(&crawler->url_queue);
    
    // 방문한 URL 집합 초기화
    url_set_init(&crawler->visited_urls);
    
    // 웹 그래프 초기화
    web_graph_init(&crawler->web_graph, max_vertices);
    
    // 크롤링 제한 설정
    crawler->max_depth = max_depth;
    crawler->max_urls = max_urls;
    crawler->crawled_count = 0;
    
    // 뮤텍스 초기화
    pthread_mutex_init(&crawler->count_mutex, NULL);
}

// 크롤링 시작 함수
void crawler_start(Crawler *crawler, const char *start_url) {
    printf("크롤링 시작: %s\n", start_url);
    printf("최대 깊이: %d, 최대 URL 수: %d\n", crawler->max_depth, crawler->max_urls);
    
    // 시작 URL을 큐에 추가
    url_queue_enqueue(&crawler->url_queue, start_url, 0);
    
    // 멀티스레딩 설정
    pthread_t threads[MAX_THREADS];
    int i;
    
    // 스레드 생성
    for (i = 0; i < MAX_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, crawler_thread, (void *)crawler) != 0) {
            fprintf(stderr, "스레드 생성 실패\n");
            return;
        }
    }
    
    // 모든 스레드 종료 대기
    for (i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("크롤링 완료. 총 %d 개의 URL 처리됨\n", crawler->crawled_count);
    
    // 결과 출력
    crawler_print_stats(crawler);
    
    // 그래프를 DOT 파일로 저장 (시각화용) - 절대 경로 사용
    char dot_file_path[4096];
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        snprintf(dot_file_path, sizeof(dot_file_path), "%s/web_graph.dot", cwd);
        printf("DOT 파일 절대 경로: %s\n", dot_file_path);
        web_graph_save_dot(&crawler->web_graph, dot_file_path);
    } else {
        fprintf(stderr, "현재 작업 디렉토리를 가져오는 데 실패했습니다.\n");
        web_graph_save_dot(&crawler->web_graph, "web_graph.dot");
    }
}

// 단일 URL 크롤링 함수
void crawler_process_url(Crawler *crawler, const char *url, int depth) {
    // 이미 방문한 URL인지 확인
    if (url_set_contains(&crawler->visited_urls, url)) {
        return;
    }
    
    // 방문한 URL로 표시
    url_set_add(&crawler->visited_urls, url);
    
    // 크롤링 카운터 증가
    pthread_mutex_lock(&crawler->count_mutex);
    crawler->crawled_count++;
    int current_count = crawler->crawled_count;
    pthread_mutex_unlock(&crawler->count_mutex);
    
    printf("[%d] 크롤링: %s (깊이: %d)\n", current_count, url, depth);
    
    // 최대 깊이 확인
    if (depth >= crawler->max_depth) {
        return;
    }
    
    // HTTP 요청 수행
    HttpResponse response;
    http_response_init(&response);
    
    if (http_get(url, &response)) {
        printf("HTTP 요청 성공: %s (응답 크기: %zu 바이트)\n", url, response.size);
        
        // HTML에서 URL 추출
        ParseResult parse_result;
        parse_result_init(&parse_result, 100);
        
        printf("URL 추출 시작...\n");
        extract_urls(response.data, url, &parse_result);
        printf("URL 추출 완료: %d개 URL 발견\n", parse_result.count);
        
        // 추출된 URL 처리
        printf("\n*** 추출된 URL 처리 시작 (%d개) ***\n", parse_result.count);
        int i;
        for (i = 0; i < parse_result.count; i++) {
            const char *found_url = parse_result.urls[i];
            printf("\n발견된 URL: %s\n", found_url);
            printf("DEBUG: URL 처리 시작: %s\n", found_url);
            
            // 웹 그래프에 간선 추가
            web_graph_add_edge(&crawler->web_graph, url, found_url);
            printf("DEBUG: 웹 그래프에 간선 추가 완료: %s -> %s\n", url, found_url);
            
            // 최대 URL 수 확인
            pthread_mutex_lock(&crawler->count_mutex);
            int should_continue = (crawler->crawled_count < crawler->max_urls);
            pthread_mutex_unlock(&crawler->count_mutex);
            printf("DEBUG: 최대 URL 수 확인 - should_continue: %d\n", should_continue);
            
            if (!should_continue) {
                printf("최대 URL 수 도달, 처리 중단\n");
                break;
            }
            
            printf("DEBUG: 방문 여부 확인 전\n");
            
            // 이 URL을 방문했는지 확인
            int already_visited = url_set_contains(&crawler->visited_urls, found_url);
            printf("DEBUG: 방문 여부 확인 후 - already_visited: %d\n", already_visited);
            printf("이미 방문했나요? %s\n", already_visited ? "예" : "아니오");
            
            if (!already_visited) {
                // 새로운 URL을 큐에 추가
                printf("DEBUG: 큐에 URL 추가 시도 전\n");
                printf("큐에 URL 추가 시도: %s (깊이: %d)\n", found_url, depth + 1);
                url_queue_enqueue(&crawler->url_queue, found_url, depth + 1);
                printf("DEBUG: 큐에 URL 추가 완료\n");
            } else {
                printf("이미 방문한 URL이므로 큐에 추가하지 않음\n");
            }
        }
        
        // 파싱 결과 정리
        parse_result_cleanup(&parse_result);
    }
    
    // HTTP 응답 정리
    http_response_cleanup(&response);
}

// 병렬 크롤링을 위한 스레드 함수
void *crawler_thread(void *arg) {
    Crawler *crawler = (Crawler *)arg;
    URLItem url_item;
    int idle_count = 0;
    
    printf("[스레드 %lu] 시작\n", (unsigned long)pthread_self());
    
    while (1) {
        // 최대 URL 수 확인
        pthread_mutex_lock(&crawler->count_mutex);
        int should_continue = (crawler->crawled_count < crawler->max_urls);
        pthread_mutex_unlock(&crawler->count_mutex);
        
        if (!should_continue) {
            printf("[스레드 %lu] 최대 URL 수 도달, 종료\n", (unsigned long)pthread_self());
            break;
        }
        
        // 큐에서 URL 가져오기
        if (url_queue_dequeue(&crawler->url_queue, &url_item)) {
            // URL 처리
            printf("[스레드 %lu] URL 처리: %s (깊이: %d)\n", 
                   (unsigned long)pthread_self(), url_item.url, url_item.depth);
            crawler_process_url(crawler, url_item.url, url_item.depth);
            idle_count = 0; // 작업을 했으므로 대기 카운터 초기화
        } else {
            // 큐가 비어있으면 잠시 대기
            idle_count++;
            
            if (idle_count % 10 == 1) { // 로그 양을 줄이기 위해 간허한 로그만 출력
                printf("[스레드 %lu] 대기 중 (아이들 카운트: %d)\n", 
                       (unsigned long)pthread_self(), idle_count);
            }
            
            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 100000000;  // 0.1초
            nanosleep(&ts, NULL);
            
            // 너무 오래 대기하면 종료 (5초 = 50회 대기)
            if (idle_count > 50) {
                printf("[스레드 %lu] 너무 오래 대기하여 종료\n", (unsigned long)pthread_self());
                break;
            }
        }
    }
    
    return NULL;
}

// 크롤링 결과 출력 함수
void crawler_print_stats(Crawler *crawler) {
    printf("\n--- 크롤링 통계 ---\n");
    printf("처리된 URL 수: %d\n", crawler->crawled_count);
    printf("방문한 URL 수: %d\n", crawler->visited_urls.count);
    printf("웹 그래프 정점 수: %d\n", crawler->web_graph.num_vertices);
    
    // 웹 그래프 정보 출력
    printf("\n--- 웹 그래프 정보 ---\n");
    web_graph_print(&crawler->web_graph);
}

// 크롤러 정리 함수 (메모리 해제)
void crawler_cleanup(Crawler *crawler) {
    // URL 큐 정리
    url_queue_cleanup(&crawler->url_queue);
    
    // 방문한 URL 집합 정리
    url_set_cleanup(&crawler->visited_urls);
    
    // 웹 그래프 정리
    web_graph_cleanup(&crawler->web_graph);
    
    // 뮤텍스 파괴
    pthread_mutex_destroy(&crawler->count_mutex);
}

#ifndef CRAWLER_H
#define CRAWLER_H

#include "common.h"
#include "url_queue.h"
#include "url_set.h"
#include "web_graph.h"
#include "html_parser.h"
#include "http_client.h"

// 크롤러 구조체
typedef struct {
    URLQueue url_queue;         // 크롤링 대기 URL 큐
    URLSet visited_urls;        // 방문한 URL 집합
    WebGraph web_graph;         // 웹 그래프
    int max_depth;              // 최대 크롤링 깊이
    int max_urls;               // 최대 크롤링 URL 수
    int crawled_count;          // 현재까지 크롤링한 URL 수
    pthread_mutex_t count_mutex; // 카운터 동기화를 위한 뮤텍스
} Crawler;

// 크롤러 초기화
void crawler_init(Crawler *crawler, int max_depth, int max_urls, int max_vertices);

// 크롤링 시작
void crawler_start(Crawler *crawler, const char *start_url);

// 단일 URL 크롤링
void crawler_process_url(Crawler *crawler, const char *url, int depth);

// 병렬 크롤링을 위한 스레드 함수
void *crawler_thread(void *arg);

// 크롤링 결과 출력
void crawler_print_stats(Crawler *crawler);

// 크롤러 정리 (메모리 해제)
void crawler_cleanup(Crawler *crawler);

#endif /* CRAWLER_H */

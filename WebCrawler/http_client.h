#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "common.h"

// HTTP 응답 구조체
typedef struct {
    char *data;        // 응답 데이터
    size_t size;       // 데이터 크기
} HttpResponse;

// HTTP 응답 초기화
void http_response_init(HttpResponse *response);

// HTTP GET 요청 수행
int http_get(const char *url, HttpResponse *response);

// HTTP 응답 정리 (메모리 해제)
void http_response_cleanup(HttpResponse *response);

// robots.txt 확인 함수
int is_url_allowed(const char *url);

#endif /* HTTP_CLIENT_H */

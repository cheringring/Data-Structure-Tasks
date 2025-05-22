#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include "common.h"

// HTML 파싱 결과 구조체
typedef struct {
    char **urls;       // 추출된 URL 배열
    int count;         // URL 개수
    int capacity;      // 배열 용량
} ParseResult;

// 파싱 결과 초기화
void parse_result_init(ParseResult *result, int initial_capacity);

// URL 추출 함수
void extract_urls(const char *html, const char *base_url, ParseResult *result);

// URL 정규화 함수 (상대 URL을 절대 URL로 변환)
void normalize_url(const char *base_url, const char *url, char *result, int max_length);

// 파싱 결과 정리 (메모리 해제)
void parse_result_cleanup(ParseResult *result);

#endif /* HTML_PARSER_H */

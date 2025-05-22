#include "html_parser.h"

// 파싱 결과 초기화 함수
void parse_result_init(ParseResult *result, int initial_capacity) {
    result->urls = (char **)malloc(initial_capacity * sizeof(char *));
    if (result->urls == NULL) {
        error("파싱 결과 메모리 할당 실패");
    }
    
    result->count = 0;
    result->capacity = initial_capacity;
}

// URL 추출 함수
void extract_urls(const char *html, const char *base_url, ParseResult *result) {
    // 간단한 HTML 파서 구현
    // 실제 구현에서는 더 강력한 HTML 파서 라이브러리 사용 권장
    
    printf("HTML 파서 시작 - 기본 URL: %s\n", base_url);
    
    if (html == NULL) {
        printf("HTML 데이터가 NULL입니다!\n");
        return;
    }
    
    // 안전한 문자열 길이 계산
    size_t html_length = 0;
    const char *p = html;
    while (*p != '\0') {
        html_length++;
        p++;
    }
    
    printf("HTML 데이터 길이: %zu 바이트\n", html_length);
    
    if (html_length == 0) {
        printf("HTML 데이터가 비어있습니다!\n");
        return;
    }
    
    // 유효한 UTF-8 문자열인지 확인
    extern int is_valid_utf8(const char *str, size_t length);
    if (!is_valid_utf8(html, html_length)) {
        printf("경고: 유효하지 않은 UTF-8 데이터 발견, 안전 모드로 파싱\n");
    }
    
    const char *href_tag = "href=\"";
    const char *href_tag_end = "\"";
    const char *current = html;
    
    while ((current = strstr(current, href_tag)) != NULL) {
        // href 속성 시작 위치로 이동
        current += strlen(href_tag);
        
        // href 속성 끝 위치 찾기
        const char *end = strstr(current, href_tag_end);
        if (end == NULL) {
            current++;
            continue;
        }
        
        // URL 추출 - 안전하게 처리
        size_t url_length = end - current;
        
        // 너무 긴 URL 처리
        if (url_length > MAX_URL_LENGTH - 1) {
            printf("경고: URL이 너무 깁니다. 자름: %zu -> %d\n", url_length, MAX_URL_LENGTH - 1);
            url_length = MAX_URL_LENGTH - 1;
        }
        
        char *url = (char *)malloc((url_length + 1) * sizeof(char));
        if (url == NULL) {
            error("메모리 할당 실패");
        }
        
        // 안전한 문자만 복사
        size_t j = 0;
        for (size_t i = 0; i < url_length; i++) {
            // ASCII 문자이거나 유효한 UTF-8 시퀀스의 일부인 경우만 복사
            unsigned char c = (unsigned char)current[i];
            if (c < 128 || (c >= 192 && c <= 247)) {
                url[j++] = current[i];
            }
        }
        url[j] = '\0';
        
        // URL 정규화
        char normalized_url[MAX_URL_LENGTH];
        normalize_url(base_url, url, normalized_url, MAX_URL_LENGTH);
        
        // 유효한 URL인지 확인 (http 또는 https로 시작하는지)
        if (strncmp(normalized_url, "http://", 7) == 0 || 
            strncmp(normalized_url, "https://", 8) == 0) {
            
            // 결과 배열 용량 확인 및 확장
            if (result->count >= result->capacity) {
                result->capacity *= 2;
                result->urls = (char **)realloc(result->urls, result->capacity * sizeof(char *));
                if (result->urls == NULL) {
                    error("파싱 결과 메모리 재할당 실패");
                }
            }
            
            // URL 저장
            result->urls[result->count] = (char *)malloc(MAX_URL_LENGTH * sizeof(char));
            if (result->urls[result->count] == NULL) {
                error("URL 메모리 할당 실패");
            }
            
            strncpy(result->urls[result->count], normalized_url, MAX_URL_LENGTH - 1);
            result->urls[result->count][MAX_URL_LENGTH - 1] = '\0';
            result->count++;
        }
        
        current = end + 1;
    }
}

// URL 정규화 함수 (상대 URL을 절대 URL로 변환)
void normalize_url(const char *base_url, const char *url, char *result, int max_length) {
    // 이미 절대 URL인 경우
    if (strncmp(url, "http://", 7) == 0 || strncmp(url, "https://", 8) == 0) {
        strncpy(result, url, max_length - 1);
        result[max_length - 1] = '\0';
        return;
    }
    
    // 기본 URL에서 도메인 추출
    char domain[MAX_URL_LENGTH];
    const char *protocol_end = strstr(base_url, "://");
    if (protocol_end == NULL) {
        strncpy(result, url, max_length - 1);
        result[max_length - 1] = '\0';
        return;
    }
    
    protocol_end += 3;  // "://" 건너뛰기
    
    const char *path_start = strchr(protocol_end, '/');
    if (path_start == NULL) {
        // 경로가 없는 경우 (예: https://example.com)
        snprintf(domain, MAX_URL_LENGTH, "%.*s", (int)(protocol_end - base_url + strlen(protocol_end)), base_url);
    } else {
        // 경로가 있는 경우 (예: https://example.com/path)
        snprintf(domain, MAX_URL_LENGTH, "%.*s", (int)(path_start - base_url), base_url);
    }
    
    // URL 유형에 따라 정규화
    if (url[0] == '/') {
        // 루트 상대 URL (예: /page.html)
        snprintf(result, max_length, "%s%s", domain, url);
    } else if (url[0] == '#' || url[0] == '?' || strncmp(url, "javascript:", 11) == 0 || 
               strncmp(url, "mailto:", 7) == 0 || strncmp(url, "tel:", 4) == 0) {
        // 앵커, 쿼리 파라미터, 자바스크립트, 메일, 전화번호 등은 무시
        result[0] = '\0';
    } else {
        // 상대 URL (예: page.html)
        const char *last_slash = strrchr(base_url, '/');
        if (last_slash == NULL || last_slash < protocol_end) {
            // 기본 URL에 경로가 없는 경우
            snprintf(result, max_length, "%s/%s", domain, url);
        } else {
            // 기본 URL의 디렉토리까지 사용
            int dir_length = last_slash - base_url + 1;
            snprintf(result, max_length, "%.*s%s", dir_length, base_url, url);
        }
    }
}

// 파싱 결과 정리 함수 (메모리 해제)
void parse_result_cleanup(ParseResult *result) {
    int i;
    
    // 각 URL 메모리 해제
    for (i = 0; i < result->count; i++) {
        free(result->urls[i]);
    }
    
    // URL 배열 메모리 해제
    free(result->urls);
    
    // 구조체 초기화
    result->urls = NULL;
    result->count = 0;
    result->capacity = 0;
}

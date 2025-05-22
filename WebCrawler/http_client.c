#include "http_client.h"

#include <curl/curl.h>
#include <ctype.h>
// 이 파일은 libcurl 라이브러리를 사용한 HTTP 클라이언트 구현입니다.
// 실제 컴파일 시에는 libcurl 라이브러리가 필요합니다.
// 설치 방법: sudo apt-get install libcurl4-openssl-dev (Ubuntu/Debian)
//           brew install curl (macOS)

// UTF-8 유효성 검사 함수
int is_valid_utf8(const char *str, size_t length) {
    if (!str || length == 0)
        return 1; // 빈 문자열은 유효함
        
    const unsigned char *bytes = (const unsigned char *)str;
    size_t i = 0;
    
    while (i < length) {
        if ((bytes[i] & 0x80) == 0) {
            // 1바이트 문자
            i += 1;
        } else if ((bytes[i] & 0xE0) == 0xC0) {
            // 2바이트 문자
            if (i + 1 >= length || (bytes[i+1] & 0xC0) != 0x80)
                return 0;
            i += 2;
        } else if ((bytes[i] & 0xF0) == 0xE0) {
            // 3바이트 문자
            if (i + 2 >= length || (bytes[i+1] & 0xC0) != 0x80 || (bytes[i+2] & 0xC0) != 0x80)
                return 0;
            i += 3;
        } else if ((bytes[i] & 0xF8) == 0xF0) {
            // 4바이트 문자
            if (i + 3 >= length || (bytes[i+1] & 0xC0) != 0x80 || 
                (bytes[i+2] & 0xC0) != 0x80 || (bytes[i+3] & 0xC0) != 0x80)
                return 0;
            i += 4;
        } else {
            return 0;
        }
    }
    return 1;
}

// HTTP 응답 콜백 함수 (libcurl에서 사용)
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    HttpResponse *response = (HttpResponse *)userp;
    
    // 응답 데이터 메모리 재할당
    char *ptr = realloc(response->data, response->size + real_size + 1);
    if (ptr == NULL) {
        fprintf(stderr, "메모리 재할당 실패\n");
        return 0;  // 실패
    }
    
    response->data = ptr;
    
    // 새 데이터 복사
    memcpy(&(response->data[response->size]), contents, real_size);
    response->size += real_size;
    response->data[response->size] = '\0';  // 문자열 끝 표시
    
    return real_size;
}

// HTTP 응답 초기화 함수
void http_response_init(HttpResponse *response) {
    response->data = malloc(1);  // 최소 크기 할당
    if (response->data == NULL) {
        error("HTTP 응답 메모리 할당 실패");
    }
    
    response->data[0] = '\0';  // 빈 문자열로 초기화
    response->size = 0;
}

// HTTP GET 요청 수행 함수
int http_get(const char *url, HttpResponse *response) {
    CURL *curl;
    CURLcode res;
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "WebCrawler/1.0");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // 최대 10초 대기
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L); // 최대 5초 연결 대기
        
        res = curl_easy_perform(curl);
        
        // Content-Type 헤더에서 charset 확인
        char *content_type = NULL;
        curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
        
        curl_easy_cleanup(curl);
        
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() 실패: %s\n", curl_easy_strerror(res));
            return FALSE;
        }
        
        // 인코딩 검사 및 처리
        if (!is_valid_utf8(response->data, response->size)) {
            printf("경고: 유효하지 않은 UTF-8 데이터 발견, 대체 인코딩 시도...\n");
            
            // 원본 데이터 백업
            char *original_data = response->data;
            size_t original_size = response->size;
            
            // 대체 방법 1: 손상된 문자 제거
            char *cleaned_data = malloc(original_size + 1);
            if (cleaned_data) {
                size_t j = 0;
                for (size_t i = 0; i < original_size; i++) {
                    // ASCII 문자이거나 유효한 UTF-8 시퀀스의 일부인 경우만 복사
                    if ((unsigned char)original_data[i] < 128 || 
                        ((unsigned char)original_data[i] >= 192 && (unsigned char)original_data[i] <= 247)) {
                        cleaned_data[j++] = original_data[i];
                    }
                }
                cleaned_data[j] = '\0';
                
                // 원본 데이터 해제하고 정제된 데이터로 교체
                free(original_data);
                response->data = cleaned_data;
                response->size = j;
            }
        }
        
        printf("HTTP 응답 받음: %s (길이: %zu 바이트)\n", url, response->size);
        printf("HTTP 응답 일부: %.100s...\n", response->size > 0 ? response->data : "(비어있음)");
        
        return TRUE;
    }
    return FALSE;
}

// HTTP 응답 정리 함수 (메모리 해제)
void http_response_cleanup(HttpResponse *response) {
    free(response->data);
    response->data = NULL;
    response->size = 0;
}

// robots.txt 확인 함수
int is_url_allowed(const char *url) {
    // 실제 구현에서는 robots.txt 파싱 및 확인 로직 구현
    // 여기서는 간단히 모든 URL 허용
    return TRUE;
}

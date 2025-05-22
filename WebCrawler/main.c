#include "crawler.h"

void print_usage() {
    printf("사용법: ./webcrawler [시작URL] [최대깊이] [최대URL수]\n");
    printf("예시: ./webcrawler https://example.com 3 100\n");
}

int main(int argc, char *argv[]) {
    // 명령행 인수 확인
    if (argc != 4) {
        print_usage();
        return 1;
    }
    
    // 인수 파싱
    const char *start_url = argv[1];
    int max_depth = atoi(argv[2]);
    int max_urls = atoi(argv[3]);
    
    // 유효성 검사
    if (max_depth <= 0 || max_urls <= 0) {
        printf("오류: 최대 깊이와 최대 URL 수는 양수여야 합니다.\n");
        print_usage();
        return 1;
    }
    
    // 크롤러 초기화
    Crawler crawler;
    crawler_init(&crawler, max_depth, max_urls, MAX_VERTICES);  // 정점 수는 MAX_VERTICES 상수 사용
    
    // 크롤링 시작
    crawler_start(&crawler, start_url);
    
    // 크롤러 정리
    crawler_cleanup(&crawler);
    
    return 0;
}

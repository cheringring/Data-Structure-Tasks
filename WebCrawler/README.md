# 자료구조를 활용한 웹 크롤러

이 프로젝트는 자료구조 수업에서 배운 개념을 활용하여 구현한 웹 크롤러입니다.

## 기능
- 시작 URL에서부터 웹 페이지를 크롤링
- BFS 알고리즘을 사용하여 링크 탐색
- 방문한 URL 관리를 위한 해시 테이블 구현
- 웹 페이지 간의 관계를 그래프로 표현
- 멀티스레딩을 통한 병렬 크롤링
- 크롤링 결과 저장 및 분석

## 컴파일 방법
```
gcc -o webcrawler main.c crawler.c url_queue.c url_set.c web_graph.c html_parser.c http_client.c -lcurl
```

## 실행 방법
```
./webcrawler [시작URL] [최대깊이] [최대URL수]
```

## 예시
```
./webcrawler https://example.com 3 100
```

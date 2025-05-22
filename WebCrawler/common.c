#include "common.h"
#include <stdio.h>
#include <stdlib.h>

// 에러 처리 함수 정의
void error(const char *message) {
    fprintf(stderr, "오류: %s\n", message);
    exit(1);
}

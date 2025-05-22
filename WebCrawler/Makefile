# 컴파일러 설정
CC = gcc
CFLAGS = -Wall -I. -pthread
LDFLAGS = -lcurl -pthread

# 소스 파일과 오브젝트 파일
SRC_DIR = .
BUILD_DIR = build
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = webcrawler

# 기본 타겟
all: $(BUILD_DIR) $(TARGET)

# 오브젝트 파일 컴파일
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 실행 파일 링크
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# 빌드 디렉토리 생성
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 정리
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean

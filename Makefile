# компилятор
CC := cc

# директории
SRC_DIR := src
OBJ_DIR := build/obj
BIN_DIR := build

# файлы
TARGET := $(BIN_DIR)/openRTS

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# флаги
CFLAGS := -Wall -Wextra -Werror -g -Iinclude
LDFLAGS := -lraylib -lm -ldl -lpthread -lX11

# цель по умолчанию
all: $(TARGET)

# линковка
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# компиляция
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# очистка
clean:
	rm -rf build

run: all
	./$(TARGET)

.PHONY: all clean

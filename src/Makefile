# 定义编译器和编译选项
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# 定义目标文件和生成的目标文件路径
OBJ_DIR = ../build
TARGET = main

# 定义目标及其依赖关系
$(TARGET): $(OBJ_DIR)/main.o $(OBJ_DIR)/lex.o $(OBJ_DIR)/parser.o
	$(CC) -o $@ $^

# 编译目标文件
$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 为lex.o和parser.o添加头文件依赖关系
$(OBJ_DIR)/lex.o $(OBJ_DIR)/parser.o: lex.h parser.h

# 清理生成的目标文件
clean:
	rm -f $(TARGET) $(OBJ_DIR)/*.o


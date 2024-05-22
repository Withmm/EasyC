# Makefile for the project

# 编译器和编译选项
CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude

# 源文件和目标文件
SRCDIR = .
BUILDDIR = ../build
TARGET = main

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

# 默认目标
all: $(BUILDDIR) $(TARGET)

# 创建build目录
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# 链接目标文件生成可执行文件
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# 编译源文件生成目标文件
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(BUILDDIR)/*.o $(TARGET)

.PHONY: all clean


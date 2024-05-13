CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

OBJ_DIR = ../build
TARGET = main

$(TARGET): $(OBJ_DIR)/main.o $(OBJ_DIR)/lex.o $(OBJ_DIR)/parser.o
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/lex.o $(OBJ_DIR)/parser.o: lex.h parser.h

clean:
	rm -f $(TARGET) $(OBJ_DIR)/*.o


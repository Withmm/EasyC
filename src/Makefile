CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude -g

SRCDIR = .
BUILDDIR = ../build
LOGDIR = ../log
EASYCDIR = ../easyc
TARGET = main

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

all: $(BUILDDIR) $(TARGET)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BUILDDIR)/*.o $(TARGET) $(EASYCDIR)/*.easyc $(LOGDIR)/*.log

.PHONY: all clean


# Makefile
# This Makefile defines the build process for the project. It includes rules for
# compiling the C source files into object files and linking them to create the
# final executable. It also provides commands for creating necessary directories,
# and cleaning up the build artifacts.
#
# Variables:
# - CC: The compiler to use (gcc).
# - CFLAGS: The compiler flags, including warning options and include directory.
# - SRCDIR: The source directory.
# - BUILDDIR: The build directory where object files are stored.
# - LOGDIR: The directory for storing log files.
# - EASYCDIR: The directory for storing easyc files.
# - TARGET: The name of the final executable.
#
# Rules:
# - all: Builds the target executable.
# - $(BUILDDIR): Creates the build directory if it doesn't exist.
# - $(TARGET): Links the object files to create the executable.
# - $(BUILDDIR)/%.o: Compiles source files into object files.
# - clean: Removes the build artifacts and log files.
#
# Special targets:
# - .PHONY: Declares that 'all' and 'clean' are not actual files.

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


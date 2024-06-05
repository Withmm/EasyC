/*
 * main.c
 * This file contains the main function which serves as the entry point for the compiler or interpreter.
 * It reads the source code from a file, performs lexical analysis to tokenize the input, parses the tokens
 * to generate an Abstract Syntax Tree (AST), and processes the AST. Finally, it prints the emitted nodes.
 * Usage: ./main <source_file>
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "include/lex.h"
#include "include/parser.h"
#include "include/ast.h"

char source_string[FILEMAX];
struct Token maintoken[NTOKEN];
int ntoken;

char typedebug[13][16] = {"Identifier", "Keyword", "Constant", "Operator", "Delimiter", "Comment", "Preprocessor", "Unknown", "Char", "Short", "Int", "Long", "Void"};

extern struct emit_node *emits[EMITSMAX];
extern int emit_count;
extern void print_emit();

int main(int argc, char *argv[]) {
        if (argc != 2) {
                fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
                return EXIT_FAILURE;
        }

        FILE *fp = fopen(argv[1], "r");
        assert(fp != NULL);

        // Read file line by line, handling comments and storing in source_string
        char line[LINE_MAX];
        int source_index = 0;
        while (fgets(line, LINE_MAX, fp) != NULL) {
                // Handle comments
                char *comment_start = strstr(line, "//");
                if (comment_start != NULL) {
                        *comment_start = '\0';
                }
                // Add to the source_string
                strncpy(source_string + source_index, line, FILEMAX - source_index);
                source_index += strlen(line);
        }
        fclose(fp);

        ntoken = lexer(source_string, maintoken);

        // Print tokens
        for (int i = 0; i < ntoken; i++) {
                printf("%s: %s", typedebug[maintoken[i].ttype], maintoken[i].lexeme);
                if (i != ntoken - 1 && maintoken[i].line != maintoken[i + 1].line) {
                        printf("\n");
                }
        }
        printf("\n");

        struct AST_node_program* ast = parser(maintoken, ntoken);

        handler_ast(ast);
        print_emit();

        return 0;
}


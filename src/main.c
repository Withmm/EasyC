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
int main(int argc, char *argv[]) 
{

	if (argc != 2) {
		printf("argument error\n");
		return -1;
	  }

	FILE *fp;
	fp = fopen(argv[1], "r");
    //printf("argv[1] = %s\n", argv[1]);
	assert(fp != NULL);
    // read line by line
    char line[LINE_MAX];
    int source_index = 0;
    while (fgets(line, LINE_MAX, fp) != NULL) {
        // handle comments
        char *comment_start = strstr(line, "//");
        if (comment_start != NULL) {
            *comment_start = '\0';
        }
        // add to the source_string
        strncpy(source_string + source_index, line, FILEMAX - source_index);
        source_index += strlen(line);
    }
    fclose(fp);
	  //printf("%s", source_string);
	ntoken = lexer(source_string, maintoken);
    /*
	for (int i = 0; i < ntoken; i++) {
		printf("%s: ", typedebug[maintoken[i].ttype]);
		printf("%s\n", maintoken[i].lexeme);
	}
    */
    struct AST_node_program* ast = parser(maintoken, ntoken);

    handler_ast(ast);
    print_emit();        
    //printf("parser is successful!\n");
	return 0;
}



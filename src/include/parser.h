#include "types.h"
int parser(struct Token *token, int ntoken);
struct qua {
    char op[4];
    char arg1[16];
    char arg2[16];
    char result[16];
};

struct AST_node {
    long type;
};
struct AST_node_program {
    struct AST_node basis;
};


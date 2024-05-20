#include "types.h"
int parser(struct Token *token, int ntoken);
char *strdup(const char *s);
struct qua {
    char op[4];
    char arg1[16];
    char arg2[16];
    char result[16];
};


#ifndef _LEX_H
#include "types.h"
int lexer(char *file_lexeme, struct Token *maintoken);
int is_letter(char c);
int is_number(char c);
int is_delimiter(char c, char *delimiter);
int is_loperator(char c, char *loperator[4]);
int is_keyword(char *str, char *keyword[16]);
int is_operator(char *src, char *loperator[4]);
#endif

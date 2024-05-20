#ifndef _TYPES_H
#define _TYPES_H
#define _LEX_H
#define FILEMAX 16384
#define LINE_MAX 1024
#define NTOKEN 16384
#define NKEYWORD 23
#define KEYWORDMAX 16
#define NDELIMITER 14
#define NOPERATOR 20
enum tokentype{Identifier = 0, Keyword, Constant, Operator, Delimiter, Comment, Preprocessor, Unknown, Char, Short, Int, Long, Void};

struct Token {	
	enum tokentype ttype;
	char lexeme[32];
};
#endif

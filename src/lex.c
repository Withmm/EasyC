#include "include/lex.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
int global_line = 1; 
char delimiter[NDELIMITER] = {
	';',
	'(',
	')',
	'{',
	'}',
	'[',
	']',
	'.',
	'<',
	'>',
	',',
	'"',
	'\\',
	':',
};

char *loperator[NOPERATOR] = {
	"+",
	"-",
	"*",
	"/",
	"%",
	"=",
	"|",
	"&",
	"<",
	">",
	"!=",
	"==",
	">=",
	"<=",
	">>",
	"<<",
	"&&",
	"||",
	"++",
	"--",
};
char *keyword[NKEYWORD]= {
    "auto",
    "break",
    "case",
    "const",
    "continue",
    "default",
    "do",
    "enum",
    "extern",
    "for",
    "goto",
    "if",
	"else",
    "int",
    "long",
    "char",
    "return",
    "short",
    "signed",
    "while",
	"include",
	"define",
	"void",
};



int lexer(char *file_lexeme, struct Token *maintoken)
{
	int ntoken = 0;
	char *pchar = file_lexeme;
	while(*pchar) {
		char first_char = *pchar;
		struct Token token;
		memset(token.lexeme, 0, 32);
		int shift = 0;
		while(1) {
			if (first_char == '#') {
				token.ttype = Preprocessor;
				token.lexeme[shift++] = *pchar++;
				break;
			} else if(is_loperator(first_char, loperator)){
				token.ttype = Operator;
				token.lexeme[shift++] = *pchar++;
				break;
				
			} else if (is_letter(first_char)) {
				token.ttype = Identifier;
				if (is_letter(*pchar) || is_number(*pchar)) {
					token.lexeme[shift++] = *pchar++;			
				}
				else {
					if (is_keyword(token.lexeme, keyword)) { 
						token.ttype = Keyword;
					}
					break;
				}

			} else if (is_number(first_char)) {
				if(is_number(*pchar)) {
					token.lexeme[shift++] = *pchar++;
				} else {
					token.ttype = Constant;
					break;
				}
			} else if (is_delimiter(first_char, delimiter)) {
				token.lexeme[shift++] = *pchar++;
				token.ttype = Delimiter;
				break;
			} else if (isspace(first_char)){
				if (first_char == '\n')
					global_line++;
				pchar++;
				break;
			} else {
				fprintf(stderr, "unknown token: %c\n", *pchar);
				pchar++;
				break;
			}
		}	
		strcpy(maintoken[ntoken].lexeme, token.lexeme);
		maintoken[ntoken].ttype = token.ttype;
		maintoken[ntoken].line = global_line;
		if (isspace(maintoken[ntoken].lexeme[0]) || maintoken[ntoken].lexeme[0] == 0)
			ntoken--;
		ntoken++;
		if (*pchar == '\n' || *pchar == ' ' || *pchar == '\t') {
			if (*pchar == '\n')
				global_line++;
			pchar++;
		}
	}

	for (int i = 0; i < ntoken; i++) {
		if (maintoken[i].ttype == Keyword) {
			if (strcmp(maintoken[i].lexeme, "char") == 0) {
				maintoken[i].ttype = Char;
			}
			if (strcmp(maintoken[i].lexeme, "short") == 0) {
				maintoken[i].ttype = Short;
			}
			if (strcmp(maintoken[i].lexeme, "int") == 0) {
				maintoken[i].ttype = Int;
			}
			if (strcmp(maintoken[i].lexeme, "long") == 0) {
				maintoken[i].ttype = Long;
			}
			if (strcmp(maintoken[i].lexeme, "void") == 0) {
				maintoken[i].ttype = Void;
			}

		}
		if (maintoken[i].ttype == Operator && !(is_operator(maintoken[i].lexeme, loperator))) {
			maintoken[i].ttype = Unknown;
			fprintf(stderr, "error : cannot identify %s\n", maintoken[i].lexeme);
			continue;
		}

		if (strcmp(maintoken[i].lexeme, "<") == 0) {
			if (strcmp(maintoken[i - 1].lexeme, "include") == 0) {
				maintoken[i].ttype = Delimiter;
			}
		}
		if (strcmp(maintoken[i].lexeme, ">") == 0) {
			if (strcmp(maintoken[i - 5].lexeme, "include") == 0) {

				maintoken[i].ttype = Delimiter;
			}
		}
	}
	return ntoken;
}

int is_letter(char c) 
{
	return isalpha(c) || c == '_';
}
int is_number(char c) 
{
	return isdigit(c);
}
int is_delimiter(char c, char *delimiter) 
{
	for (int i = 0; i < NDELIMITER; i++) {
		if (delimiter[i] == c) {
			return 1;
		}
	}
	return 0;
}
int is_keyword(char *str, char *keyword[16]) 
{
	for (int i = 0; i < NKEYWORD; i++) {
		if (strcmp(str, keyword[i]) == 0) {
			return 1;
		}
	}	
	return 0;
}

int is_loperator(char c, char *loperator[4]) 
{
	for (int i = 0; i < NOPERATOR; i++) {
		if (loperator[i][0] == c) {
			return 1;
		}
	}
	return 0;
}

int is_operator(char *src, char *loperator[4])
{
	for (int i = 0; i < NOPERATOR; i++) {
		if (strcmp(src, loperator[i]) == 0) {
			return 1;
		}
	}
	return 0;
}

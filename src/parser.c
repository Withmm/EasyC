#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void errormsg(const char *str)
{
	fprintf(stderr, "%s: error\n", str);
	exit(-1);
}
/* declaration */
int maxtoken;
int program(struct Token *token, int curtoken);
int declaration_list(struct Token *token, int curtoken);
int declaration(struct Token *token, int curtoken);
int var_declaration(struct Token *token, int curtoken);
int func_declaration(struct Token *token, int curtoken);
int type(struct Token *token, int curtoken);
int identifiler(struct Token *token, int curtoken);
int parameters(struct Token *token, int curtoken);
int stmt(struct Token *token, int curtoken);
int paras(struct Token *token, int curtoken);
int para_list(struct Token *token, int curtoken);
int param(struct Token *token, int curtoken);
int _para_list(struct Token *token, int curtoken);
/*
E->TE’
E’->+TE’ | ε
T->FT’
T’->*F T’| ε
F->(E) | i
from: https://www.omegaxyz.com/2018/12/21/ll1-recursiondown/
*/
int expr(struct Token *token, int curtoken); // E
int _expr(struct Token *token, int curtoken); // E'
int __expr(struct Token *token, int curtoken); // T
int ___expr(struct Token *token, int curtoken); // T'
int expr_t(struct Token *token, int curtoken); // F

int state(struct Token *token, int curtoken);
int state_if(struct Token *token, int curtoken);
int state_for(struct Token *token, int curtoken);
int state_let(struct Token *token, int curtoken);
int state_switch(struct Token *token, int curtoken);
int state_return(struct Token *token, int curtoken);
int program(struct Token *token, int curtoken);
int program(struct Token *token, int curtoken)
{
	curtoken = declaration_list(token, curtoken);

	if (curtoken != -1) {
		return curtoken;
	} else {
		errormsg("program");
	}
	return curtoken;
}

int declaration_list(struct Token *token, int curtoken)
{
	while(curtoken < maxtoken){
		printf("token : %d\n", curtoken);
		curtoken = declaration(token, curtoken);
		if (strcmp(token[curtoken].lexeme, ";") == 0) {
			curtoken++;
		}
	}
	return curtoken;
}

int declaration(struct Token *token, int curtoken)
{

//	int main() or int x;  
	if (strcmp(token[curtoken + 2].lexeme, "(") == 0) {
		curtoken = func_declaration(token, curtoken);
	} else {
		curtoken = var_declaration(token, curtoken);
	}

	if (curtoken == -1) {
		errormsg("declaration");
	}
	return curtoken;
}

int func_declaration(struct Token *token, int curtoken) 
{

	// int 
	switch(token[curtoken].ttype) {

		case Char:				break;

		case Short:				break;

		case Int:				break;

		case Long:				break;

		case Void:				break;

		default:				errormsg("func_declaration");
	}

	curtoken++;
	//main func ....

	if (token[curtoken].ttype != Identifier) {
		errormsg("func_declaration");
	}

	curtoken++;
	// (

	curtoken = paras(token, curtoken);
	
	curtoken = stmt(token, curtoken);

	return curtoken;
}

int var_declaration(struct Token *token, int curtoken)
{
	printf("var_declaration first token:%s\n", token[curtoken].lexeme);
	// int
	switch(token[curtoken].ttype) {

		case Char:				break;

		case Short:				break;

		case Int:				break;

		case Long:				break;

		default:				errormsg("var_declaration:type");
	}


	curtoken++;
	// x

	if (token[curtoken].ttype != Identifier) {
		errormsg("var_declaration:Identifier");
	}
	
	curtoken++;
	if (strcmp(token[curtoken].lexeme, ";") == 0) {
	// ;
		return curtoken;
	}
	// =
	if (strcmp(token[curtoken].lexeme, "=") != 0) {
		errormsg("var_declaration");
	}

	curtoken++;
	curtoken = expr(token, curtoken);
	return curtoken;
}
int paras(struct Token *token, int curtoken)
{
	//(
	if (strcmp(token[curtoken].lexeme, "(") != 0) {
		errormsg("paras: missing (");
	}	

	curtoken++;
	//)
	if (strcmp(token[curtoken].lexeme, ")") == 0) {
		curtoken++;
		return curtoken;
	}

	while (1) {
		switch(token[curtoken].ttype) {

		case Char:				break;

		case Short:				break;

		case Int:				break;

		case Long:				break;

		default:				errormsg("paras");
		}
		curtoken++;
		//Identifier
	
		if (token[curtoken].ttype != Identifier) {
			errormsg("paras");
		}

		curtoken++;
		// , or )

		if (strcmp(token[curtoken].lexeme, ",") == 0) {
			curtoken++;
		} else if (strcmp(token[curtoken].lexeme, ")") == 0) {	
			curtoken++;
			return curtoken;
		} else {
			errormsg("paras");
		}
	}	
	return curtoken;
}

int stmt(struct Token *token, int curtoken) 
{
	// {
	if (strcmp(token[curtoken].lexeme, "{") != 0) {
		errormsg("stmt: no {");
	}
	curtoken++;
	while(strcmp(token[curtoken].lexeme, "}") != 0) {
		curtoken = state(token, curtoken);
		if (strcmp(token[curtoken].lexeme, ";") != 0) {
			printf("curtoken = %d\n", curtoken);
			printf("error symbol = %s\n", token[curtoken].lexeme);
			errormsg("stmt: missing ;");
		}
		curtoken++;
	}
	// }

	curtoken++;
	return curtoken;
}

int state(struct Token *token, int curtoken)
{
	if (strcmp(token[curtoken].lexeme, "return") == 0) {
		curtoken = state_return(token, curtoken);
	}

	switch(token[curtoken].ttype) {

	case Char:				

	case Short:			

	case Int:				

	case Long:				curtoken = state_let(token, curtoken);

	default:				break;
	}

	return curtoken;
}

int state_let(struct Token *token, int curtoken)
{
	// int
	switch(token[curtoken].ttype) {

	case Char:				break;

	case Short:				break;

	case Int:				break;

	case Long:				break;

	default:				printf("token = %s\n", token[curtoken].lexeme);errormsg("state_let:type");
	}

	curtoken++;
	//Identifier

	if (token[curtoken].ttype != Identifier) {
		errormsg("state_let");
	}

	curtoken++;
	if (strcmp(token[curtoken].lexeme, ";") == 0) {
	// ;
		return	curtoken;
	}
	// =

	if (strcmp(token[curtoken].lexeme, "=") != 0) {
		errormsg("state_let: missing =");
	}

	curtoken++;
	//expr
	curtoken = expr(token, curtoken);
	return curtoken;
}

int state_return(struct Token *token, int curtoken)
{
	// return
	if (strcmp(token[curtoken].lexeme, "return") != 0) {
		errormsg("state_return: missing return");
	}

	curtoken++;
	// expr
	if (token[curtoken].ttype != Constant) {
		errormsg("state_return: missing constant");
	}
	curtoken++;
	// ;
	return curtoken;
}

int expr(struct Token *token, int curtoken)
{
	curtoken = __expr(token, curtoken);	
	curtoken = _expr(token, curtoken);
	return curtoken;
}

int _expr(struct Token *token, int curtoken)
{
	if (strcmp(token[curtoken].lexeme, "+") == 0 ) {
		curtoken++;
		curtoken = __expr(token, curtoken);
		curtoken = _expr(token, curtoken);
	}
	return curtoken;
}

int __expr(struct Token *token, int curtoken)
{
	curtoken = expr_t(token, curtoken);
	curtoken = ___expr(token, curtoken);
	return curtoken;
}

int ___expr(struct Token *token, int curtoken)
{
	if (strcmp(token[curtoken].lexeme, "*") == 0) {
		curtoken++;
		curtoken = expr_t(token, curtoken);
		curtoken = ___expr(token, curtoken);
	}
	return curtoken;
}
int expr_t(struct Token *token, int curtoken)
{
	if (token[curtoken].ttype == Identifier || token[curtoken].ttype == Constant) {
		curtoken++;
		return curtoken;
	}
	if (strcmp(token[curtoken].lexeme, "(") == 0) {
		curtoken++;
		curtoken = expr(token, curtoken);
		if (strcmp(token[curtoken].lexeme, ")") != 0) {
			errormsg("expr error: missing \')\'\n");
		}
		curtoken++;
		return curtoken;
	}
	errormsg("expr error: missing Identifier or \'(\'\n");
	return curtoken;
}



int parser(struct Token *token, int ntoken)
{
	maxtoken = ntoken;
	int curtoken = 0;
	return program(token, curtoken);	
}

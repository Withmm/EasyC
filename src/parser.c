#include "include/types.h"
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
F->(E) | constant | Identifier | func
func->Identifer(paras)
from: https://www.omegaxyz.com/2018/12/21/ll1-recursiondown/
*/
int expr(struct Token *token, int curtoken); // E
int _expr(struct Token *token, int curtoken); // E'
int expr_T(struct Token *token, int curtoken); // T
int expr_T_(struct Token *token, int curtoken); // T'
int expr_t(struct Token *token, int curtoken); // F
int expr_constant(struct Token *token, int curtoken);
int expr_variable(struct Token *token, int curtoken);
int expr_func(struct Token *token, int curtoken);
int expr_func_paras(struct Token *token, int curtoken);
int expr_bool(struct Token *token, int curtoken);

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
	}
	return curtoken;
}

int declaration(struct Token *token, int curtoken)
{

//	int main() or int x;  

	if (strcmp(token[curtoken + 2].lexeme, "(") == 0) {
		printf("get in func_declaration\n");
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
		curtoken++;
		return curtoken;
	}
	// =
	if (strcmp(token[curtoken].lexeme, "=") != 0) {
		errormsg("var_declaration");
	}

	curtoken++;
	// constant
	if (token[curtoken].ttype != Constant) {
		errormsg("global var_declaration error: missing Constant");
	}

	curtoken++;
	// ;
	if (strcmp(token[curtoken].lexeme, ";") != 0) {
		errormsg("global var_declaration error: missing ;");
	}
	curtoken++;
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
	}
	// }
	curtoken++;
	return curtoken;
}

int state(struct Token *token, int curtoken)
{

	// return xxx
	if (strcmp(token[curtoken].lexeme, "return") == 0) {
		curtoken = state_return(token, curtoken);
		return curtoken;
	}

	// int xx = expr;
	// xx = expr;
	switch(token[curtoken].ttype) {

	case Char:				

	case Short:			

	case Int:				

	case Long:				curtoken = state_let(token, curtoken);
							return curtoken;

	case Identifier:
							if (strcmp(token[curtoken + 1].lexeme, "=") == 0) {
								curtoken += 2;
								curtoken = expr(token, curtoken); 
								//;
								curtoken++;
								return curtoken;
							} else {
								errormsg("state error: missing =");
							}

	default:				break;
	}
	// if statement
	if (strcmp(token[curtoken].lexeme, "if") == 0) {
		curtoken = state_if(token, curtoken);
		return curtoken;
	}
	return curtoken;
}

int state_let(struct Token *token, int curtoken)
{
	printf("get in state_let\n");
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
		curtoken++;
		return curtoken;
	}
	// =

	if (strcmp(token[curtoken].lexeme, "=") != 0) {
		for (int i = curtoken; i < maxtoken; i++) {
			printf("%s ", token[i].lexeme);
		}
		errormsg("state_let: missing =");
	}

	curtoken++;
	//expr
	curtoken = expr(token, curtoken);

	//;
	curtoken++;
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
	curtoken = expr(token, curtoken);	
	//;
	if (strcmp(token[curtoken].lexeme, ";") != 0 ) {
		errormsg("state_return error: missing ;");
	}
	curtoken++;
	printf("return to -> %s\n", token[curtoken].lexeme);
	return curtoken;
}

int expr(struct Token *token, int curtoken)
{
	curtoken = expr_T(token, curtoken);	
	curtoken = _expr(token, curtoken);
	return curtoken;
}

int _expr(struct Token *token, int curtoken)
{
	if (strcmp(token[curtoken].lexeme, "+") == 0 ) {
		curtoken++;
		curtoken = expr_T(token, curtoken);
		curtoken = _expr(token, curtoken);
		return curtoken;
	}

	if (strcmp(token[curtoken].lexeme, "-") == 0 ) {
		curtoken++;
		curtoken = expr_T(token, curtoken);
		curtoken = _expr(token, curtoken);
		return curtoken;
	}
	return curtoken;
}

int expr_T(struct Token *token, int curtoken)
{
	curtoken = expr_t(token, curtoken);
	curtoken = expr_T_(token, curtoken);
	return curtoken;
}

int expr_T_(struct Token *token, int curtoken)
{
	if (strcmp(token[curtoken].lexeme, "*") == 0) {
		curtoken++;
		curtoken = expr_t(token, curtoken);
		curtoken = expr_T_(token, curtoken);
	}
	if (strcmp(token[curtoken].lexeme, "/") == 0) {
		curtoken++;
		curtoken = expr_t(token, curtoken);
		curtoken = expr_T_(token, curtoken);
	}
	return curtoken;
}
int expr_t(struct Token *token, int curtoken)
{
	if (token[curtoken].ttype == Constant) {
		curtoken = expr_constant(token, curtoken);
		return curtoken;
	}
	if (token[curtoken].ttype == Identifier && strcmp(token[curtoken + 1].lexeme, "(") != 0) {
		curtoken = expr_variable(token, curtoken);
		return curtoken;
	}
	if (token[curtoken].ttype == Identifier) {
		curtoken = expr_func(token, curtoken);
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

int expr_constant(struct Token *token, int curtoken)
{
	if (token[curtoken].ttype == Constant) {
		curtoken++;
		return curtoken;
	} else {
		errormsg("expr error: missing constant");
		exit(-1);
	}
}

int expr_variable(struct Token *token, int curtoken)
{
	if (token[curtoken].ttype == Identifier) {
		curtoken++;
		return curtoken;
	} else {
		errormsg("expr error: missing variable");
		exit(-1);
	}
}

int expr_func(struct Token *token, int curtoken)
{
	if (token[curtoken].ttype != Identifier) {
		errormsg("expr error: missing func_name");
	}			
	curtoken++;
	//(
	
	if (strcmp(token[curtoken].lexeme, "(") != 0) {
		errormsg("expr_func error: missing ("); 
	}
	curtoken++;

	curtoken = expr_func_paras(token, curtoken);
	return curtoken;
}

int expr_func_paras(struct Token *token, int curtoken)
{
	while(strcmp(token[curtoken].lexeme, ")") != 0) {
		switch (token[curtoken].ttype) {
		case Identifier:
			curtoken++;
			break;
		case Constant:
			curtoken++;
			break;
		default:
			errormsg("expr_func_paras error: expected Identifier or Constant");
			break;
		}
		if (strcmp(token[curtoken].lexeme, ",") == 0) {
			curtoken++;
		}
		if (strcmp(token[curtoken].lexeme, ")") == 0) {
			break;
		}
	}
	curtoken++;
	return curtoken;
}

int state_if(struct Token *token, int curtoken)
{
	if (strcmp(token[curtoken].lexeme, "if") != 0) {
		errormsg("state_if error: missing if");
	}	
	curtoken++;
	//(
	if (strcmp(token[curtoken].lexeme, "(") != 0) {
		errormsg("state_if error: missing (");
	}
	curtoken++;	
	curtoken = expr_bool(token, curtoken);
	//)
	if (strcmp(token[curtoken].lexeme, ")") != 0 ) {
		errormsg("state_if error: missing )");
	}
	curtoken++;
	// {
	curtoken = stmt(token, curtoken);
	return curtoken;
}

int expr_bool(struct Token *token, int curtoken) {
	curtoken = expr(token, curtoken);
	if (strcmp(token[curtoken].lexeme, ">") == 0) {
		if (strcmp(token[curtoken + 1].lexeme, "=") == 0) {
			curtoken += 2;
			curtoken = expr(token, curtoken);
			return curtoken;
		}
		curtoken++;
		curtoken = expr(token, curtoken);
		return curtoken;
		
	}
	if (strcmp(token[curtoken].lexeme, "<") == 0) {
		if (strcmp(token[curtoken + 1].lexeme, "=") == 0) {
			curtoken += 2;
			curtoken = expr(token, curtoken);
			return curtoken;
		}
		curtoken++;
		curtoken = expr(token, curtoken);
		return curtoken;	
	}
	if (strcmp(token[curtoken].lexeme, "=") == 0) {
		if (strcmp(token[curtoken + 1].lexeme, "=") != 0) {
			errormsg("expr_bool error: use == instead =");
		}
		curtoken += 2;
		curtoken = expr(token, curtoken);
		return curtoken;
	}
	if (strcmp(token[curtoken].lexeme, "!") == 0) {
		if (strcmp(token[curtoken + 1].lexeme, "=") != 0) {
			errormsg("expr_bool error: unexpected symbol '!'");
		}
		curtoken += 2;
		curtoken = expr(token, curtoken);
		return curtoken;
	}
	errormsg("no match bool symbol: != == > < >= <=");
	exit(-1);
}
int parser(struct Token *token, int ntoken)
{
	maxtoken = ntoken;
	int curtoken = 0;
	return program(token, curtoken);	
}

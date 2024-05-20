#include "include/types.h"
#include "include/parser.h"
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
int curtoken;
int program(struct Token *token);
int declaration_list(struct Token *token);
int declaration(struct Token *token);
int var_declaration(struct Token *token);
int func_declaration(struct Token *token);
int type(struct Token *token);
int identifiler(struct Token *token);
int parameters(struct Token *token);
int stmt(struct Token *token);
int paras(struct Token *token);
int para_list(struct Token *token);
int param(struct Token *token);
int _para_list(struct Token *token);
/*
E->TE’
E’->+TE’ | ε
T->FT’
T’->*F T’| ε
F->(E) | constant | Identifier | func
func->Identifer(paras)
from: https://www.omegaxyz.com/2018/12/21/ll1-recursiondown/
*/
int expr(struct Token *token); // E
int _expr(struct Token *token); // E'
int expr_T(struct Token *token); // T
int expr_T_(struct Token *token); // T'
int expr_t(struct Token *token); // F
int expr_constant(struct Token *token);
int expr_variable(struct Token *token);
int expr_func(struct Token *token);
int expr_func_paras(struct Token *token);
int expr_bool(struct Token *token);

int state(struct Token *token);
int state_if(struct Token *token);
int state_for(struct Token *token);
int state_let(struct Token *token);
int state_switch(struct Token *token);
int state_return(struct Token *token);
int program(struct Token *token);

int program(struct Token *token)
{
	curtoken = declaration_list(token);

	if (curtoken != -1) {
		return curtoken;
	} else {
		errormsg("program");
	}
	return curtoken;
}

int declaration_list(struct Token *token)
{
	while(curtoken < maxtoken){
		curtoken = declaration(token);
	}
	return curtoken;
}

int declaration(struct Token *token)
{

//	int main() or int x;  

	if (strcmp(token[curtoken + 2].lexeme, "(") == 0) {
		printf("get in func_declaration\n");
		curtoken = func_declaration(token);
	} else {
		curtoken = var_declaration(token);
	}

	if (curtoken == -1) {
		errormsg("declaration");
	}
	return curtoken;
}

int func_declaration(struct Token *token) 
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

	curtoken = paras(token);
	curtoken = stmt(token);
	return curtoken;
}

int var_declaration(struct Token *token)
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
int paras(struct Token *token)
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

int stmt(struct Token *token) 
{
	// {
	if (strcmp(token[curtoken].lexeme, "{") != 0) {
		errormsg("stmt: no {");
	}
	curtoken++;
	while(strcmp(token[curtoken].lexeme, "}") != 0) {
		curtoken = state(token);
	}
	// }

	curtoken++;
	return curtoken;
}

int state(struct Token *token)
{

	// return xxx
	if (strcmp(token[curtoken].lexeme, "return") == 0) {
		curtoken = state_return(token);
		return curtoken;
	}

	// int xx = expr;
	// xx = expr;
	switch(token[curtoken].ttype) {

	case Char:				

	case Short:			

	case Int:				

	case Long:				curtoken = state_let(token);
							return curtoken;

	case Identifier:
							if (strcmp(token[curtoken + 1].lexeme, "=") == 0) {
								curtoken += 2;
								curtoken = expr(token); 
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
		curtoken = state_if(token);
		return curtoken;
	}
	//for statement
	if (strcmp(token[curtoken].lexeme, "for") == 0) {
		curtoken = state_for(token);
		return curtoken;
	}
	return curtoken;
}

int state_let(struct Token *token)
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
	curtoken = expr(token);

	//;
	if (strcmp(token[curtoken].lexeme, ";") != 0 && strcmp(token[curtoken].lexeme, ")") != 0) {
		errormsg("state_let error: missing ; or )");
	}
	curtoken++;
	return curtoken;
}

int state_return(struct Token *token)
{
	// return
	if (strcmp(token[curtoken].lexeme, "return") != 0) {
		errormsg("state_return: missing return");
	}

	curtoken++;
	// expr
	curtoken = expr(token);	
	//;
	if (strcmp(token[curtoken].lexeme, ";") != 0 ) {
		errormsg("state_return error: missing ;");
	}
	curtoken++;
	printf("return to -> %s\n", token[curtoken].lexeme);
	return curtoken;
}

int expr(struct Token *token)
{
	curtoken = expr_T(token);	
	curtoken = _expr(token);
	return curtoken;
}

int _expr(struct Token *token)
{
	if (strcmp(token[curtoken].lexeme, "+") == 0 ) {
		curtoken++;
		curtoken = expr_T(token);
		curtoken = _expr(token);
		return curtoken;
	}

	if (strcmp(token[curtoken].lexeme, "-") == 0 ) {
		curtoken++;
		curtoken = expr_T(token);
		curtoken = _expr(token);
		return curtoken;
	}
	return curtoken;
}

int expr_T(struct Token *token)
{
	curtoken = expr_t(token);
	curtoken = expr_T_(token);
	return curtoken;
}

int expr_T_(struct Token *token)
{
	if (strcmp(token[curtoken].lexeme, "*") == 0) {
		curtoken++;
		curtoken = expr_t(token);
		curtoken = expr_T_(token);
	}
	if (strcmp(token[curtoken].lexeme, "/") == 0) {
		curtoken++;
		curtoken = expr_t(token);
		curtoken = expr_T_(token);
	}
	return curtoken;
}
int expr_t(struct Token *token)
{
	if (token[curtoken].ttype == Constant) {
		curtoken = expr_constant(token);
		return curtoken;
	}
	if (token[curtoken].ttype == Identifier && strcmp(token[curtoken + 1].lexeme, "(") != 0) {
		curtoken = expr_variable(token);
		return curtoken;
	}
	if (token[curtoken].ttype == Identifier) {
		curtoken = expr_func(token);
		return curtoken;
	}
	if (strcmp(token[curtoken].lexeme, "(") == 0) {
		curtoken++;
		curtoken = expr(token);
		if (strcmp(token[curtoken].lexeme, ")") != 0) {
			errormsg("expr error: missing \')\'\n");
		}
		curtoken++;
		return curtoken;
	}
	errormsg("expr error: missing Identifier or \'(\'\n");
	return curtoken;
}

int expr_constant(struct Token *token)
{
	if (token[curtoken].ttype == Constant) {
		curtoken++;
		return curtoken;
	} else {
		errormsg("expr error: missing constant");
		exit(-1);
	}
}

int expr_variable(struct Token *token)
{
	if (token[curtoken].ttype == Identifier) {
		curtoken++;
		return curtoken;
	} else {
		errormsg("expr error: missing variable");
		exit(-1);
	}
}

int expr_func(struct Token *token)
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

	curtoken = expr_func_paras(token);
	return curtoken;
}

int expr_func_paras(struct Token *token)
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

int state_if(struct Token *token)
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
	curtoken = expr_bool(token);
	//)
	if (strcmp(token[curtoken].lexeme, ")") != 0 ) {
		errormsg("state_if error: missing )");
	}
	curtoken++;
	// {

	curtoken = stmt(token);
	//else or end
	if (strcmp(token[curtoken].lexeme, "else") == 0) {
		curtoken++;
		curtoken = stmt(token);
	}
	return curtoken;
}

int expr_bool(struct Token *token) {
	curtoken = expr(token);
	if (strcmp(token[curtoken].lexeme, ">") == 0) {
		if (strcmp(token[curtoken + 1].lexeme, "=") == 0) {
			curtoken += 2;
			curtoken = expr(token);
			return curtoken;
		}
		curtoken++;
		curtoken = expr(token);
		return curtoken;
		
	}
	if (strcmp(token[curtoken].lexeme, "<") == 0) {
		if (strcmp(token[curtoken + 1].lexeme, "=") == 0) {
			curtoken += 2;
			curtoken = expr(token);
			return curtoken;
		}
		curtoken++;
		curtoken = expr(token);
		return curtoken;	
	}
	if (strcmp(token[curtoken].lexeme, "=") == 0) {
		if (strcmp(token[curtoken + 1].lexeme, "=") != 0) {
			errormsg("expr_bool error: use == instead =");
		}
		curtoken += 2;
		curtoken = expr(token);
		return curtoken;
	}
	if (strcmp(token[curtoken].lexeme, "!") == 0) {
		if (strcmp(token[curtoken + 1].lexeme, "=") != 0) {
			errormsg("expr_bool error: unexpected symbol '!'");
		}
		curtoken += 2;
		curtoken = expr(token);
		return curtoken;
	}
	errormsg("no match bool symbol: != == > < >= <=");
	exit(-1);
}

int state_for(struct Token *token)
{
	if (strcmp(token[curtoken].lexeme, "for") != 0) {
		errormsg("state_for error: missing for");
	}
	curtoken++;
	//(

	curtoken++;

	curtoken = state_let(token);
	
	if (strcmp(token[curtoken].lexeme, ";") == 0) { // for (int x = 0; ; ....
		curtoken++;
	} else { // for (int x = 0; x < 10
		curtoken = expr_bool(token);
		if (strcmp(token[curtoken].lexeme, ";") != 0) {
			errormsg("state_for error: missing ;");
		}
		curtoken++;
		switch(token[curtoken].ttype) {

		case Char:				

		case Short:			

		case Int:				

		case Long:	
			curtoken = state_let(token);
			// {

		case Identifier:
			if (strcmp(token[curtoken + 1].lexeme, "=") == 0) {
					curtoken += 2;
					curtoken = expr(token); 
					//;
					curtoken++;
					//{
			} else {
					errormsg("state error: missing =");
			}

		default: 
			break;
		}
	}
	
	// {
	curtoken = stmt(token);
	return curtoken;
}
int parser(struct Token *token, int ntoken)
{
	maxtoken = ntoken;
	return program(token);	
}

#include "include/types.h"
#include "include/parser.h"
#include "include/ast.h"
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
struct AST_node_declaration_list *declaration_list(struct Token *token);
struct AST_node_declarations *declaration(struct Token *token);
struct AST_node_var_dec_only *var_declaration(struct Token *token);
struct AST_node_func_dec *func_declaration(struct Token *token);
int type(struct Token *token);
int identifiler(struct Token *token);
int parameters(struct Token *token);
struct AST_node_stmt *stmt(struct Token *token);
struct AST_node_func_paras *paras(struct Token *token);
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
struct AST_node_expr *expr(struct Token *token); // E
struct AST_node_expr_ *_expr(struct Token *token); // E'
struct AST_node_expr_T *expr_T(struct Token *token); // T
struct AST_node_expr_T_ *expr_T_(struct Token *token); // T'
struct AST_node_expr_t *expr_t(struct Token *token); // F
struct AST_node_func_call *expr_func(struct Token *token);
struct AST_node_condition *expr_bool(struct Token *token);

struct AST_node_state *state(struct Token *token);
struct AST_node_state_if *state_if(struct Token *token);
struct AST_node_state_for *state_for(struct Token *token);
struct AST_node_state_dec *state_dec(struct Token *token);
struct AST_node_state_let *state_let(struct Token *token);
struct AST_node_state *state_switch(struct Token *token);
struct AST_node_state_return*state_return(struct Token *token);
int program(struct Token *token, struct AST_node_program *program_node);

int program(struct Token *token, struct AST_node_program *program_node)
{
    program_node->basis = malloc(sizeof(struct AST_node));
	program_node->basis->type = PROGRAM;
	program_node->dec = declaration_list(token);
	if (curtoken != -1) {
		return curtoken;
	} else {
		errormsg("program");
	}
	return curtoken;
}

struct AST_node_declaration_list *declaration_list(struct Token *token) {
    struct AST_node_declaration_list *dec_list = malloc(sizeof(struct AST_node_declaration_list));
    if (!dec_list) {
        perror("Failed to allocate memory for declaration list");
        exit(EXIT_FAILURE);
    }

    dec_list->count = 0;
    dec_list->dec_capacity = 10;  // 初始容量
    dec_list->basis = malloc(sizeof(struct AST_node));
    if (!dec_list->basis) {
        perror("Failed to allocate memory for basis");
        exit(EXIT_FAILURE);
    }
    dec_list->basis->type = DECLARATION_LIST;
    dec_list->declarations = malloc(dec_list->dec_capacity * sizeof(struct AST_node_declaration *));
    if (!dec_list->declarations) {
        perror("Failed to allocate memory for declarations array");
        exit(EXIT_FAILURE);
    }

    while (curtoken < maxtoken) {
        if (dec_list->count >= dec_list->dec_capacity) {
            dec_list->dec_capacity *= 2;
            dec_list->declarations = realloc(dec_list->declarations, dec_list->dec_capacity * sizeof(struct AST_node_declaration *));
            if (!dec_list->declarations) {
                perror("Failed to reallocate memory for declarations array");
                exit(EXIT_FAILURE);
            }
        }
        dec_list->declarations[dec_list->count] = declaration(token);
        dec_list->count++;
    }
    return dec_list;
}

struct AST_node_declarations *declaration(struct Token *token)
{

//	int main() or int x;  
	struct AST_node_declarations *dec = malloc(sizeof (struct AST_node_declarations));
    dec->basis = malloc (sizeof (struct AST_node));
	if (strcmp(token[curtoken + 2].lexeme, "(") == 0) {
		dec->basis->type = FUNC_DEC;
		dec->real_dec.func = func_declaration(token);
	} else {
		dec->basis->type = VAR_DEC;
		dec->real_dec.var = var_declaration(token);
	}

	if (curtoken == -1) {
		errormsg("declaration");
	}
	return dec;
}

struct AST_node_func_dec *func_declaration(struct Token *token) 
{
	struct AST_node_func_dec *func_dec = malloc(sizeof (struct AST_node_func_dec));
    func_dec->basis = malloc(sizeof (struct AST_node));
	func_dec->basis->type = FUNC_DEC;
	// int 
	switch(token[curtoken].ttype) {

	case Char:				
		func_dec->func_type = INT; 	
		break;

	case Short:			
		func_dec->func_type = SHORT;
		break;

	case Int:				
		func_dec->func_type = INT;
		break;
							
	case Long:				
		func_dec->func_type = LONG;
		break;

	case Void:				
		func_dec->func_type = VOID;
		break;

	default:				
		errormsg("func_declaration");

	}

	curtoken++;
	//main func ....

	if (token[curtoken].ttype != Identifier) {
		errormsg("func_declaration");
	}

	func_dec->func_name = strdup(token[curtoken].lexeme);
	curtoken++;
	// (

	func_dec->params = paras(token);
	func_dec->stmt = stmt(token);
	return func_dec;
}

struct AST_node_var_dec_only *var_declaration(struct Token *token)
{
	struct AST_node_var_dec_only *var_dec_only = malloc(sizeof (struct AST_node_var_dec_only));
	var_dec_only->basis = malloc(sizeof (struct AST_node));
	var_dec_only->basis->type = VAR_DEC;
	var_dec_only->init_val = 0;
	// int
	switch(token[curtoken].ttype) {

	case Char:				
		var_dec_only->var_type = VCHAR;
		break;

	case Short:				
		var_dec_only->var_type = VSHORT;
		break;

	case Int:				
		var_dec_only->var_type = VINT;
		break;

	case Long:				
		var_dec_only->var_type = VLONG;
		break;

	default:				errormsg("var_declaration:type");
	}


	curtoken++;
	// x

	if (token[curtoken].ttype != Identifier) {
		errormsg("var_declaration:Identifier");
	}
	var_dec_only->var_name = strdup(token[curtoken].lexeme);

	curtoken++;
	if (strcmp(token[curtoken].lexeme, ";") == 0) {
	// ;
		curtoken++;
		return var_dec_only;
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
	var_dec_only->init_val = atoi(token[curtoken].lexeme);

	// ;
	if (strcmp(token[curtoken].lexeme, ";") != 0) {
		errormsg("global var_declaration error: missing ;");
	}
	curtoken++;

	return var_dec_only;
}
struct AST_node_func_paras *paras(struct Token *token) {

    struct AST_node_func_paras *params = malloc(sizeof(struct AST_node_func_paras));

    if (!params) {
        perror("Failed to allocate memory for func_paras");
        exit(EXIT_FAILURE);
    }

    params->basis = malloc(sizeof(struct AST_node));
    if (!params->basis) {
        perror("Failed to allocate memory for basis");
        exit(EXIT_FAILURE);
    }
    params->basis->type = PARAMS;
    params->paras_count = 0;
    params->paras_capacity = 5;
    params->paras_name = malloc(params->paras_capacity * sizeof(char *));
    params->paras_type = malloc(params->paras_capacity * sizeof(enum func_type_enum));
    if (!params->paras_name || !params->paras_type) {
        perror("Failed to allocate memory for parameters");
        exit(EXIT_FAILURE);
    }

    // (
    if (strcmp(token[curtoken].lexeme, "(") != 0) {
        errormsg("paras: missing (");
    }

    curtoken++;
    // )
    if (strcmp(token[curtoken].lexeme, ")") == 0) {
        curtoken++;
        return params;
    }

    while (1) {
        if (params->paras_count >= params->paras_capacity) {
            params->paras_capacity *= 2;
            params->paras_name = realloc(params->paras_name, params->paras_capacity * sizeof(char *));
            params->paras_type = realloc(params->paras_type, params->paras_capacity * sizeof(enum func_type_enum));
            if (!params->paras_name || !params->paras_type) {
                perror("Failed to reallocate memory for parameters");
                exit(EXIT_FAILURE);
            }
        }

        switch (token[curtoken].ttype) {
            case Char:
                params->paras_type[params->paras_count] = VCHAR;
                break;
            case Short:
                params->paras_type[params->paras_count] = VSHORT;
                break;
            case Int:
                params->paras_type[params->paras_count] = VINT;
                break;
            case Long:
                params->paras_type[params->paras_count] = VLONG;
                break;
            default:
                errormsg("paras: invalid type");
        }

        curtoken++;
        // Identifier
        if (token[curtoken].ttype != Identifier) {
            errormsg("paras: missing identifier");
        }

        params->paras_name[params->paras_count] = strdup(token[curtoken].lexeme);
        if (!params->paras_name[params->paras_count]) {
            perror("Failed to allocate memory for parameter name");
            exit(EXIT_FAILURE);
        }

        params->paras_count++;
        curtoken++;

        // , or )
        if (strcmp(token[curtoken].lexeme, ",") == 0) {
            curtoken++;
        } else if (strcmp(token[curtoken].lexeme, ")") == 0) {
            curtoken++;
            return params;
        } else {
            errormsg("paras: missing , or )");
        }
    }
    return params;
}

struct AST_node_stmt *stmt(struct Token *token) {
    struct AST_node_stmt *stmt_node = malloc(sizeof(struct AST_node_stmt));
    if (!stmt_node) {
        perror("Failed to allocate memory for stmt_node");
        exit(EXIT_FAILURE);
    }

    stmt_node->basis = malloc(sizeof(struct AST_node));
    if (!stmt_node->basis) {
        perror("Failed to allocate memory for basis");
        exit(EXIT_FAILURE);
    }
    stmt_node->basis->type = STMT;

    stmt_node->state_capacity = 10;
    stmt_node->state_count = 0;
    stmt_node->state = malloc(stmt_node->state_capacity * sizeof(struct AST_node_state *));
    if (!stmt_node->state) {
        perror("Failed to allocate memory for state array");
        exit(EXIT_FAILURE);
    }

    // {
    if (strcmp(token[curtoken].lexeme, "{") != 0) {
        errormsg("stmt: no {");
    }

    curtoken++;
    while (strcmp(token[curtoken].lexeme, "}") != 0) {
        if (stmt_node->state_count >= stmt_node->state_capacity) {
            stmt_node->state_capacity *= 2;
            stmt_node->state = realloc(stmt_node->state, stmt_node->state_capacity * sizeof(struct AST_node_state *));
            if (!stmt_node->state) {
                perror("Failed to reallocate memory for state array");
                exit(EXIT_FAILURE);
            }
        }

        stmt_node->state[stmt_node->state_count] = state(token);
        stmt_node->state_count++;
    }

    // }
    curtoken++;
    return stmt_node;
}

struct AST_node_state *state(struct Token *token)
{
    struct AST_node_state *state_node = malloc(sizeof (struct AST_node_state));
    state_node->basis = malloc(sizeof (struct AST_node));
    state_node->basis->type = STATE;
	// return xxx
	if (strcmp(token[curtoken].lexeme, "return") == 0) {
        state_node->state_type = RETURN;
        state_node->real_state.real_return = state_return(token);
		return state_node;
	}

	// int xx = expr;
	// xx = expr;
	switch(token[curtoken].ttype) {

	case Char:				

	case Short:			

	case Int:				

    case Long:// dec statement: int x; int x = 1;
        state_node->state_type = INIT;
        state_node->real_state.real_dec = state_dec(token);        
		return state_node;

	case Identifier: // let statement int x = y;
		if (strcmp(token[curtoken + 1].lexeme, "=") == 0) {
            state_node->state_type = LET;
            state_node->real_state.real_let = state_let(token);
			return state_node;
		} else {
		    errormsg("state error: missing =");
		}

	default:				
        break;

	}
	// if statement
	if (strcmp(token[curtoken].lexeme, "if") == 0) {
        state_node->state_type = IF;
        state_node->real_state.real_if = state_if(token);
		return state_node;
	}
	//for statement
	if (strcmp(token[curtoken].lexeme, "for") == 0) {
        state_node->state_type = FOR;
        state_node->real_state.real_for = state_for(token);
		return state_node;
	}

    errormsg("no matching statement type");
    exit(-1);
}

struct AST_node_state_dec *state_dec(struct Token *token)
{
    struct AST_node_state_dec *state_dec_node = malloc(sizeof (struct AST_node_state_dec));
    state_dec_node->init_val = 0;
	// int
	switch(token[curtoken].ttype) {

	case Char:
        state_dec_node->var_type = VCHAR;
        break;

	case Short:	
        state_dec_node->var_type = VSHORT;
        break;

	case Int:
        state_dec_node->var_type = VINT;
        break;

	case Long:
        state_dec_node->var_type = VLONG;
        break;

	default:
        errormsg("state_dec:type");
	}

	curtoken++;
	//Identifier

	if (token[curtoken].ttype != Identifier) {
		errormsg("state_dec");
	}
    state_dec_node->var_name = strdup(token[curtoken].lexeme);
	curtoken++;
	if (strcmp(token[curtoken].lexeme, ";") == 0) {
	// ;
		curtoken++;
		return state_dec_node;
	}
	// =

	if (strcmp(token[curtoken].lexeme, "=") != 0) {
		errormsg("state_dec: missing =");
	}

	curtoken++;
	//expr
	state_dec_node->init_val = expr(token);

	//;
	if (strcmp(token[curtoken].lexeme, ";") != 0 && strcmp(token[curtoken].lexeme, ")") != 0) {
		errormsg("state_dec error: missing ; or )");
	}
	curtoken++;
	return state_dec_node;
}

struct AST_node_state_return *state_return(struct Token *token)
{
    struct AST_node_state_return *state_return_node = malloc(sizeof (struct AST_node_state_return));
	// return
	if (strcmp(token[curtoken].lexeme, "return") != 0) {
		errormsg("state_return: missing return");
	}

	curtoken++;
	// expr
	state_return_node->ret_val = expr(token);	

	//;
	if (strcmp(token[curtoken].lexeme, ";") != 0 ) {
		errormsg("state_return error: missing ;");
	}
	curtoken++;
	return state_return_node;
}

struct AST_node_state_let *state_let(struct Token *token)
{
    struct AST_node_state_let *state_let_node = malloc(sizeof (struct AST_node_state_let));
    
    if (token[curtoken].ttype != Identifier) {
        errormsg("state_let error: missing left value");
    }
    state_let_node->var_name = strdup(token[curtoken].lexeme);

    curtoken++;
    //=
    if (strcmp(token[curtoken].lexeme, "=") != 0) {
        errormsg("state_let error: missing =");
    } 
    curtoken++;
    //expr
    state_let_node->var_expr = expr(token);
    if (strcmp(token[curtoken].lexeme, ";") != 0 && strcmp(token[curtoken].lexeme, ")") != 0) {
        errormsg("state_let error: missing ';' or ')'");
    }
    curtoken++;
    return state_let_node;
    
}

struct AST_node_expr *expr(struct Token *token) {
    struct AST_node_expr *node = malloc(sizeof(struct AST_node_expr));
    node->expr_T = expr_T(token);
    node->expr_ = _expr(token);
    return node;
}

struct AST_node_expr_ *_expr(struct Token *token) {
    if (strcmp(token[curtoken].lexeme, "+") == 0 || strcmp(token[curtoken].lexeme, "-") == 0) {
        struct AST_node_expr_ *node = malloc(sizeof(struct AST_node_expr_));
        node->op = token[curtoken].lexeme[0];
        curtoken++;
        node->expr_T = expr_T(token);
        node->expr_ = _expr(token);
        return node;
    }
    return NULL; // Indicating no further continuation
}
struct AST_node_expr_T *expr_T(struct Token *token) {
    struct AST_node_expr_T *node = malloc(sizeof(struct AST_node_expr_T));
    node->expr_t = expr_t(token);
    node->expr_T_ = expr_T_(token);
    return node;
}

struct AST_node_expr_T_ *expr_T_(struct Token *token) {
    if (strcmp(token[curtoken].lexeme, "*") == 0 || strcmp(token[curtoken].lexeme, "/") == 0) {
        struct AST_node_expr_T_ *node = malloc(sizeof(struct AST_node_expr_T_));
        node->op = token[curtoken].lexeme[0];
        curtoken++;
        node->expr_t = expr_t(token);
        node->expr_T_ = expr_T_(token);
        return node;
    }
    return NULL; // Indicating no further continuation
}

struct AST_node_expr_t *expr_t(struct Token *token)
{
    struct AST_node_expr_t *expr_t_node = malloc(sizeof(struct AST_node_expr_t));
	if (token[curtoken].ttype == Constant) {
        expr_t_node->type = CONSTANT;
        expr_t_node->data.val = atoi(token[curtoken].lexeme);
        curtoken++;
		return expr_t_node;
	}
	if (token[curtoken].ttype == Identifier && strcmp(token[curtoken + 1].lexeme, "(") != 0) {
		expr_t_node->type = VARIABLE;
        expr_t_node->data.var_name = strdup(token[curtoken].lexeme);
        curtoken++;
		return expr_t_node;
	}
	if (token[curtoken].ttype == Identifier) {
        expr_t_node->type = FUNCTION_CALL;
        expr_t_node->data.func_call = expr_func(token);
		return expr_t_node;
	}
	if (strcmp(token[curtoken].lexeme, "(") == 0) {
        expr_t_node->type = PARENTHESIZED_EXPR;
        //(
		curtoken++;
		expr_t_node->data.expr = expr(token);
		if (strcmp(token[curtoken].lexeme, ")") != 0) {
			errormsg("expr error: missing \')\'\n");
		}
		curtoken++;
		return expr_t_node;
	}
	errormsg("expr error: missing Identifier or \'(\'\n");
    exit(-1);
}
struct AST_node_func_call *expr_func(struct Token *token) {
    struct AST_node_func_call *node = malloc(sizeof(struct AST_node_func_call));
    if (!node) {
        errormsg("Memory allocation failed for AST_node_func_call");
    }

    node->params_capacity = 5;
    node->params_count = 0;
    node->params = malloc(node->params_capacity * sizeof(struct AST_node_expr *));
    if (!node->params) {
        errormsg("Memory allocation failed for function parameters");
    }

    if (token[curtoken].ttype != Identifier) {
        errormsg("expr error: missing func_name");
    }

    node->func_name = strdup(token[curtoken].lexeme);
    if (!node->func_name) {
        errormsg("Memory allocation failed for func_name");
    }

    curtoken++;
    // (
    if (strcmp(token[curtoken].lexeme, "(") != 0) {
        errormsg("expr_func error: missing '('");
    }

    curtoken++; // Skip '('

    while (strcmp(token[curtoken].lexeme, ")") != 0) {
        if (node->params_count >= node->params_capacity) {
            node->params_capacity *= 2;
            node->params = realloc(node->params, node->params_capacity * sizeof(struct AST_node_expr *));
            if (!node->params) {
                errormsg("Memory reallocation failed for function parameters");
            }
        }
        struct AST_node_expr *param = expr(token); // Parse the parameter
        node->params[node->params_count++] = param;

        if (strcmp(token[curtoken].lexeme, ",") == 0) {
            curtoken++;
        } else if (strcmp(token[curtoken].lexeme, ")") != 0) {
            errormsg("expr_func_paras error: expected ',' or ')'");
        }
    }

    curtoken++; // Skip ')'
    return node;
}

struct AST_node_state_if *state_if(struct Token *token) {
    struct AST_node_state_if *node = malloc(sizeof(struct AST_node_state_if));
    if (!node) {
        errormsg("Memory allocation failed for AST_node_state_if");
    }

    if (strcmp(token[curtoken].lexeme, "if") != 0) {
        errormsg("state_if error: missing 'if'");
    }

    curtoken++;
    if (strcmp(token[curtoken].lexeme, "(") != 0) {
        errormsg("state_if error: missing '('");
    }

    curtoken++;
    node->condition = expr_bool(token);  // Parse the boolean expression

    if (strcmp(token[curtoken].lexeme, ")") != 0) {
        errormsg("state_if error: missing ')'");
    }

    curtoken++;
    node->if_body = stmt(token);  // Parse the statement for the if body

    node->else_body = NULL;  // Initialize else_body as NULL

    if (strcmp(token[curtoken].lexeme, "else") == 0) {
        curtoken++;
        node->else_body = stmt(token);  // Parse the statement for the else body
    }

    return node;
}
struct AST_node_condition *expr_bool(struct Token *token) {
    struct AST_node_condition *cond_node = malloc(sizeof(struct AST_node_condition));
    if (!cond_node) {
        errormsg("Memory allocation failed for AST_node_condition");
    }

    cond_node->left = expr(token);  // Parse left-hand side expression

    if (strcmp(token[curtoken].lexeme, ">") == 0) {
        if (strcmp(token[curtoken + 1].lexeme, "=") == 0) {
            cond_node->op = strdup(">=");
            curtoken += 2;
        } else {
            cond_node->op = strdup(">");
            curtoken++;
        }
    } else if (strcmp(token[curtoken].lexeme, "<") == 0) {
        if (strcmp(token[curtoken + 1].lexeme, "=") == 0) {
            cond_node->op = strdup("<=");
            curtoken += 2;
        } else {
            cond_node->op = strdup("<");
            curtoken++;
        }
    } else if (strcmp(token[curtoken].lexeme, "=") == 0) {
        if (strcmp(token[curtoken + 1].lexeme, "=") != 0) {
            errormsg("expr_bool error: use == instead of =");
        }
        cond_node->op = strdup("==");
        curtoken += 2;
    } else if (strcmp(token[curtoken].lexeme, "!") == 0) {
        if (strcmp(token[curtoken + 1].lexeme, "=") != 0) {
            errormsg("expr_bool error: unexpected symbol '!'");
        }
        cond_node->op = strdup("!=");
        curtoken += 2;
    } else {
        errormsg("no matching boolean symbol: != == > < >= <=");
    }

    cond_node->right = expr(token);  // Parse right-hand side expression

    return cond_node;
}
struct AST_node_state_for *state_for(struct Token *token) {
    struct AST_node_state_for *for_node = malloc(sizeof(struct AST_node_state_for));
    if (!for_node) {
        errormsg("Memory allocation failed for AST_node_state_for");
    }

    if (strcmp(token[curtoken].lexeme, "for") != 0) {
        errormsg("state_for error: missing for");
    }
    curtoken++;

    if (strcmp(token[curtoken].lexeme, "(") != 0) {
        errormsg("state_for error: missing (");
    }
    curtoken++;

    for_node->init = state_dec(token);

    if (strcmp(token[curtoken].lexeme, ";") != 0) {
        for_node->cond = expr_bool(token);
        if (strcmp(token[curtoken].lexeme, ";") != 0) {
            errormsg("state_for error: missing ; after condition");
        }
    } else {
        for_node->cond = NULL;
    }
    curtoken++;
    
    for_node->update = state_let(token);

    if (strcmp(token[curtoken].lexeme, "{") != 0) {
        errormsg("state_for error: missing { for loop body");
    }
    for_node->body = stmt(token);

    return for_node;
}
int parser(struct Token *token, int ntoken)
{
	maxtoken = ntoken;
	struct AST_node_program *program_node = malloc(sizeof(struct AST_node_program));
	return program(token, program_node);	
}

#include "include/ast.h"
#include "include/symbol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define DEBUG printf
char *strdup(const char *src);
symbol_table_entry *symbol_t[TABLE_SIZE];

int pc;
int data_p = 0xffff;
int data[4096];
int scope_count;
static const char *var_type_str[4] = {
    "char",
    "short",
    "int",
    "long",
};
static const char *func_type_str[5] = {
    "void",
    "char",
    "short",
    "int",
    "long",
};
void put_digital(int val);
int  get_digital(long address);
void emit(char *op, char *arg1, char *arg2, char *result);
void walk_program(struct AST_node_program* pro);
void walk_declation_list(struct AST_node_declaration_list* dec_list);
void walk_func_dec(struct AST_node_func_dec* func_dec);
void walk_stmt(struct AST_node_stmt *stmt, char *scope);
int  walk_expr(struct AST_node_expr *expr, char *scope);
int  walk_expr_(struct AST_node_expr_ *expr_, char *scope);
int  walk_expr_T(struct AST_node_expr_T *expr_T, char *scope);
int  walk_expr_T_(struct AST_node_expr_T_ *expr_T_, char *scope);
int  walk_expr_t(struct AST_node_expr_t *expr_t, char *scope);
int  function_call(struct AST_node_func_call *func);
void walk_declation_list(struct AST_node_declaration_list* dec_list) {
    for (int i = 0; i < dec_list->count; i++) {
        if (dec_list->declarations[i]->basis->type == VAR_DEC) { // global var_declaration
            struct AST_node_var_dec_only *tmp = dec_list->declarations[i]->real_dec.var;
            if (tmp->init_val) {
                char init_val_str[20];
                sprintf(init_val_str, "%ld", tmp->init_val);
                emit("=", init_val_str, NULL, tmp->var_name);
                insert_symbol(symbol_t, 
                        strdup(init_val_str), 
                        strdup(var_type_str[tmp->var_type]), 
                        "global", 
                        data_p);
                put_digital(tmp->init_val);
            } else {
                insert_symbol(symbol_t, 
                        tmp->var_name, 
                        (char *)var_type_str[tmp->var_type],
                        "global", 
                        data_p);
                put_digital(0);
                emit("=", "0", NULL, tmp->var_name);
            }
        }else if (dec_list->declarations[i]->basis->type == FUNC_DEC) { // func_declaration
			struct AST_node_func_dec *func_dec = dec_list->declarations[i]->real_dec.func;
            insert_symbol(symbol_t, 
                    func_dec->func_name,
                    (char *)func_type_str[func_dec->func_type],
                        "global", 
                        pc);
			walk_func_dec(func_dec);				
		}
    }
}

void walk_func_dec(struct AST_node_func_dec* func_dec)
{
    for (int i = 0; i < func_dec->params->paras_count; i++) {
        insert_symbol(symbol_t,
                func_dec->params->paras_name[i], 
                (char *)var_type_str[func_dec->params->paras_type[i]],
                func_dec->func_name,
                0xffffffff);
    }

    walk_stmt(func_dec->stmt, func_dec->func_name);

}
void walk_program(struct AST_node_program* pro) {
    walk_declation_list(pro->dec);
}

void walk_stmt(struct AST_node_stmt* stmt, char *scope) // in function
{
    //DEBUG("stmt: state_count-> %ld", stmt->state_count);
    if (scope == NULL) {
        scope = malloc(sizeof(char) * 16);
        memset(scope, 0, 16);
        strcpy(scope, "scope");
        char buf[16];
        memset(buf, 0, 16);
        sprintf(buf, "%d", scope_count++);
        strcat(scope, buf);
    }
    long count = stmt->state_count;
    for (int i = 0; i < count; i++) {
        struct AST_node_state *state_tmp = stmt->state[i];
        switch(state_tmp->state_type) {
        case INIT:
            if (state_tmp->real_state.real_dec->init_val == NULL) { // int x;
                emit("=", 0, NULL, state_tmp->real_state.real_dec->var_name);
                insert_symbol(symbol_t,
                        state_tmp->real_state.real_dec->var_name,
                        (char *)var_type_str[state_tmp->real_state.real_dec->var_type],
                        scope,
                        data_p);
                put_digital(0);
            } else { //int x = expr();
                int expr_val = walk_expr(state_tmp->real_state.real_dec->init_val, scope);
                char buf[16];
                memset(buf, 0, 16);
                sprintf(buf, "%d", expr_val);
                emit("=", buf, NULL, state_tmp->real_state.real_dec->var_name);
                insert_symbol(symbol_t,
                        state_tmp->real_state.real_dec->var_name,
                        (char *)var_type_str[state_tmp->real_state.real_dec->var_type],
                        scope,
                        data_p
                      );
                put_digital(expr_val);
            }
            break;
        case LET:
            break;
        case IF:
            break;
        case FOR:
            break;
        case RETURN:
            break;
        }
    }
}
int walk_expr(struct AST_node_expr *expr, char *scope) {
    if (expr == NULL) return 0;
    
    int val_T = walk_expr_T(expr->expr_T, scope);
    
    int val_E_ = walk_expr_(expr->expr_, scope);
    
    if (expr->expr_) {
        if (expr->expr_->op == '+') {
            expr->val = val_T + val_E_;
        } else if (expr->expr_->op == '-') {
            expr->val = val_T - val_E_;
        }
    } else {
        expr->val = val_T;
    }
    
    return expr->val;
}
int walk_expr_(struct AST_node_expr_ *expr_, char *scope) {
    if (expr_ == NULL) return 0;
    
    int val_T = walk_expr_T(expr_->expr_T, scope);
    
    int val_E_ = walk_expr_(expr_->expr_, scope);
    
    if (expr_->expr_) {
        if (expr_->op == '+') {
            expr_->val = val_T + val_E_;
        } else if (expr_->op == '-') {
            expr_->val = val_T - val_E_;
        }
    } else {
        expr_->val = val_T;
    }
    
    return expr_->val;
}
int walk_expr_T(struct AST_node_expr_T *expr_T, char *scope) {
    if (expr_T == NULL) return 0;
    
    int val_t = walk_expr_t(expr_T->expr_t, scope);
    
    int val_T_ = walk_expr_T_(expr_T->expr_T_, scope);
    
    if (expr_T->expr_T_) {
        if (expr_T->expr_T_->op == '*') {
            expr_T->val = val_t * val_T_;
        } else if (expr_T->expr_T_->op == '/') {
            if (val_T_ == 0)
                val_T_ = 1;
            expr_T->val = val_t / val_T_;
        }
    } else {
        expr_T->val = val_t;
    }
    
    return expr_T->val;
}
int walk_expr_T_(struct AST_node_expr_T_ *expr_T_, char *scope) {
    if (expr_T_ == NULL) return 0;
    
    int val_t = walk_expr_t(expr_T_->expr_t, scope);
    
    int val_T_ = walk_expr_T_(expr_T_->expr_T_, scope);
    
    if (expr_T_->expr_T_) {
        if (expr_T_->op == '*') {
            expr_T_->val = val_t * val_T_;
        } else if (expr_T_->op == '/') {
            if (val_T_ == 0) 
                val_T_ = 1;
            expr_T_->val = val_t / val_T_;
        }
    } else {
        expr_T_->val = val_t;
    }
    
    return expr_T_->val;
}
int walk_expr_t(struct AST_node_expr_t *expr_t, char *scope) {
    if (expr_t == NULL) return 0;
    
    switch (expr_t->type) {
        case CONSTANT:
            expr_t->val = expr_t->data.val;
            break;
        case VARIABLE:
            expr_t->val = get_digital(find_symbol(symbol_t, expr_t->data.var_name, scope)->address);
            break;
        case FUNCTION_CALL:
            expr_t->val = function_call(expr_t->data.func_call);
            break;
        case PARENTHESIZED_EXPR:
            expr_t->val = walk_expr(expr_t->data.expr, scope);
            break;
        default:
            printf("Unknown expression type in walk_expr_t");
            break;
    }
    
    return expr_t->val;
}

void handler_ast(struct AST_node_program* pro) {
    //init_symbol_table(symbol_t, TABLE_SIZE);

    walk_program(pro);
    return;
}
int function_call(struct AST_node_func_call *func)
{
    (void)func;
    return 0;
}
void emit(char* op, char* arg1, char* arg2, char* result) {
    printf("(%s, %s, %s, %s)", op, arg1 ? arg1 : "_", arg2 ? arg2 : "_", result ? result : "_");
	printf("->%d\n", pc);
    pc++;
}

void put_digital(int val)
{
    data[data_p - 0xffff] = val;
    data_p++;
}

int get_digital(long address)
{
    return data[address];
}

#include "include/ast.h"
#include "include/symbol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEBUG printf
#define EMITSMAX 1024

char *strdup(const char *src);

symbol_table_entry *symbol_t[TABLE_SIZE];
struct emit_node *emits[EMITSMAX];
struct AST_node_func_dec *current_func;
int pc = 1;
long data_p = 0xffff;
long paras_p = 0xffffff;
long paras_data[4096];
int data[4096];
int scope_count;
int emit_count;
int temp_var_count;
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
static inline char *new_temp();
static inline void backfill();
static inline void jmp_emit_helper(char *eft_val, char *right_val, char *op);
static inline void put_digital(int val, long address);
static inline void put_paras_data(int val, long address);
static int for_helper(struct AST_node_state_for *for_node, char *scope, long addr);
static int if_helper(struct AST_node_state_if *if_node, char *scope, long addr);
static int return_helper(struct AST_node_state_return *return_node, char *scope, long addr);
void emit(char *op, char *arg1, char *arg2, char *result);
void print_emit();
void walk_program(struct AST_node_program* pro);
void walk_declation_list(struct AST_node_declaration_list* dec_list);
void walk_func_dec(struct AST_node_func_dec* func_dec);
void walk_stmt(struct AST_node_stmt *stmt, char *scope, long addr);
int  walk_expr(struct AST_node_expr *expr, char *scope, long addr, char *tmp);
int  walk_expr_(struct AST_node_expr_ *expr_, char *scope, long addr, char *tmp);
int  walk_expr_T(struct AST_node_expr_T *expr_T, char *scope, long addr, char *tmp);
int  walk_expr_T_(struct AST_node_expr_T_ *expr_T_, char *scope, long addr, char *tmp);
int  walk_expr_t(struct AST_node_expr_t *expr_t, char *scope, long addr, char *tmp);
int  function_call(struct AST_node_func_call *func, char *scope, long addr);
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
                put_digital(tmp->init_val, data_p);
                data_p++;
            } else {
                insert_symbol(symbol_t, 
                        tmp->var_name, 
                        (char *)var_type_str[tmp->var_type],
                        "global", 
                        data_p);
                put_digital(0, data_p);
                data_p++;
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
    func_dec->para_addr = paras_p;
    for (int i = 0; i < func_dec->params->paras_count; i++) {
        char tmp_name[16] = "a\0";
        char to_add[16];
        memset(to_add, 0, 8);
        sprintf(to_add, "%d", i);
        strncat(tmp_name, to_add, 8);
        insert_symbol(symbol_t,
                tmp_name, 
                (char *)var_type_str[func_dec->params->paras_type[i]],
                func_dec->func_name,
                paras_p);
        paras_p++;
    }
    current_func = func_dec;
    walk_stmt(func_dec->stmt, func_dec->func_name, func_dec->para_addr);

}
void walk_program(struct AST_node_program* pro) {
    walk_declation_list(pro->dec);
    print_emit();
}

void walk_stmt(struct AST_node_stmt* stmt, char *scope, long para_addr) // in function
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
                put_digital(0, data_p);
                data_p++;
            } else { //int x = expr();
                char *result = new_temp();
                walk_expr(state_tmp->real_state.real_dec->init_val, scope, para_addr, result);
                emit("=", result, NULL, state_tmp->real_state.real_dec->var_name);
                insert_symbol(symbol_t,
                        state_tmp->real_state.real_dec->var_name,
                        (char *)var_type_str[state_tmp->real_state.real_dec->var_type],
                        scope,
                        data_p
                      );
                data_p++;
            }
            break;
        case LET: 
            (void) stmt; //modify warning
            symbol_table_entry *p = find_symbol(symbol_t, state_tmp->real_state.real_let->var_name, scope);
            if (p == NULL) {
                int find = 0;
                for (int i = 0; i < current_func->params->paras_count; i++) { //paras
                    if (strcmp(current_func->params->paras_name[i], state_tmp->real_state.real_let->var_name) == 0) {
                        char tmp_name[16] = "a\0";
                        char name_buf[16];
                        memset(name_buf, 0, 16);
                        sprintf(name_buf, "%d", i);
                        strncat(tmp_name, name_buf, 8);
                        int expr_val = walk_expr(state_tmp->real_state.real_let->var_expr, scope, para_addr, new_temp());
                        char buf[16];
                        memset(buf, 0, 16);
                        sprintf(buf, "%d", expr_val);
                        put_paras_data(expr_val, para_addr + i);
                        emit("=", buf, NULL, tmp_name);
                        find = 1;
                        break;
                    }
                }
                if (find)
                    break;
                printf("Undefined variable -> %s\n", state_tmp->real_state.real_let->var_name);
                exit(-1);
            }
            // stack 
            long addr = p->address;
            int expr_val = walk_expr(state_tmp->real_state.real_let->var_expr, scope, addr, new_temp());
            char buf[16];
            memset(buf, 0, 16);
            sprintf(buf, "%d", expr_val);
            if (addr >= 0xffffff) {
                printf("unexpected error\n");
            } else {
                put_digital(expr_val, addr);
                emit("=", buf, NULL, state_tmp->real_state.real_let->var_name); 
            }
            break;
        case IF:
            if_helper(state_tmp->real_state.real_if, scope, addr);
            break;
        case FOR:
            for_helper(state_tmp->real_state.real_for, scope, addr);
            break;
        case RETURN:
            return_helper(state_tmp->real_state.real_return, scope, addr);
            break;
        }
    }
}
char* new_temp() {
    char* temp = malloc(20);
    sprintf(temp, "t%d", temp_var_count++);
    return temp;
}


int walk_expr(struct AST_node_expr *expr, char *scope, long addr, char *result) {

    if (expr == NULL) return 0;
    char *tmp2 = new_temp();
    int val_T = walk_expr_T(expr->expr_T, scope, addr, tmp2);

    struct AST_node_expr_ *current_expr_ = expr->expr_;
    while (current_expr_) {
        char *tmp = new_temp();
        int val_E_ = walk_expr_T(current_expr_->expr_T, scope, addr, tmp);
        if (current_expr_->op == '+') {
            val_T += val_E_;
            emit("+", tmp2, tmp, result);
        } else if (current_expr_->op == '-') {
            val_T -= val_E_;
            emit("-", tmp2, tmp, result);
        }
        strcpy(tmp2, result);
        current_expr_ = current_expr_->expr_;
    }

    expr->val = val_T;
    emit("=", tmp2, NULL, result);
    return expr->val;
}

int walk_expr_(struct AST_node_expr_ *expr_, char *scope, long addr, char *result) {
    if (expr_ == NULL) return 0;
    char *tmp2 = new_temp();
    int val_T = walk_expr_T(expr_->expr_T, scope, addr, tmp2);

    struct AST_node_expr_ *current_expr_ = expr_->expr_;
    while (current_expr_) {
        char *tmp = new_temp();
        int val_E_ = walk_expr_T(current_expr_->expr_T, scope, addr, tmp);
        if (current_expr_->op == '+') {
            val_T += val_E_;
            emit("+", tmp2, tmp, result);
        } else if (current_expr_->op == '-') {
            val_T -= val_E_;
            emit("-", tmp2, tmp, result);
        }
        strcpy(tmp2, result);
        current_expr_ = current_expr_->expr_;
    }

    expr_->val = val_T;
    emit("=", tmp2, NULL, result);
    return expr_->val;
}

int walk_expr_T(struct AST_node_expr_T *expr_T, char *scope, long addr, char *result) {
    if (expr_T == NULL) return 0;
    char *tmp2 = new_temp();
    int val_t = walk_expr_t(expr_T->expr_t, scope, addr, tmp2);

    struct AST_node_expr_T_ *current_expr_T_ = expr_T->expr_T_;
    while (current_expr_T_) {
        char *tmp = new_temp();
        int val_T_ = walk_expr_t(current_expr_T_->expr_t, scope, addr, tmp);
        if (current_expr_T_->op == '*') {
            val_t *= val_T_;
            emit("*", tmp2, tmp, result);
        } else if (current_expr_T_->op == '/') {
            val_t /= val_T_;
            emit("/", tmp2, tmp, result);
        }
        strcpy(tmp2, result);
        current_expr_T_ = current_expr_T_->expr_T_;
    }

    expr_T->val = val_t;
    emit("=", tmp2, NULL, result);
    return expr_T->val;
}

int walk_expr_T_(struct AST_node_expr_T_ *expr_T_, char *scope, long addr, char *result) {
    if (expr_T_ == NULL) return 0;
    char *tmp2 = new_temp();
    int val_t = walk_expr_t(expr_T_->expr_t, scope, addr, tmp2);
    struct AST_node_expr_T_ *current_expr_T_ = expr_T_->expr_T_;
    while (current_expr_T_) {
        char *tmp = new_temp();
        int val_T_ = walk_expr_t(current_expr_T_->expr_t, scope, addr, tmp);
        if (current_expr_T_->op == '*') {
            val_t *= val_T_;
            emit("*", tmp2, tmp, result);
        } else if (current_expr_T_->op == '/') {
            val_t /= val_T_;
            emit("/", tmp2, tmp, result);
        }
        strcpy(tmp2, result);
        current_expr_T_ = current_expr_T_->expr_T_;
    }

    expr_T_->val = val_t;
    emit("=", tmp2, NULL, result);
    return expr_T_->val;
}

int walk_expr_t(struct AST_node_expr_t *expr_t, char *scope, long addr, char *result) {
    if (expr_t == NULL) return 0;

    switch (expr_t->type) {
        case CONSTANT:
            expr_t->val = expr_t->data.val;
            sprintf(result, "%d", expr_t->val);
            break;
        case VARIABLE:
            (void)scope;
            symbol_table_entry *p = find_symbol(symbol_t, expr_t->data.var_name, scope);
            if (p == NULL) {
                int find = 0;
                for (int i = 0; i < current_func->params->paras_count; i++) { //paras
                    if (strcmp(current_func->params->paras_name[i], expr_t->data.var_name) == 0) {
                        sprintf(result, "a%d", i);
                        find = 1;
                        break;
                    }
                }
                if (find)
                    break;
                printf("Undefined variable -> %s\n", expr_t->data.var_name);
                exit(-1);
            }
            sprintf(result, "%s", expr_t->data.var_name);
            break;
        case FUNCTION_CALL:
            expr_t->val = function_call(expr_t->data.func_call, scope, addr);
            emit("=", "a7", NULL, result);
            break;
        case PARENTHESIZED_EXPR:
            (void) result; //modify warning
            char *tmp = new_temp();
            expr_t->val = walk_expr(expr_t->data.expr, scope, addr, tmp);
            emit("=", tmp, NULL, result);
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

int function_call(struct AST_node_func_call *func, char *scope, long addr)
{
    for (int i = 0; i < func->params_count; i++) {
        char tmp_name[16] = "a\0";
        char to_add[16];
        memset(to_add, 0, 8);
        sprintf(to_add, "%d", i);
        strncat(tmp_name, to_add, 8);
        char *tmp = new_temp();
        walk_expr(func->params[i], scope, addr, tmp);
        emit("=", tmp, NULL, tmp_name);
    }
    symbol_table_entry *func_p = find_symbol(symbol_t, func->func_name, "global");
    if (func_p == NULL) {
        printf("Undefined function name -> %s\n", func->func_name);
    }
    emit("call", NULL, NULL, func->func_name);
    return 0;
}

void emit(char* op, char* arg1, char* arg2, char* result) {
    struct emit_node *node = malloc(sizeof(struct emit_node));    
    node->op = op ? strdup(op) : NULL;
    node->arg1 = arg1 ? strdup(arg1) : NULL;
    node->arg2 = arg2 ? strdup(arg2) : NULL;
    node->result = result ? strdup(result) : NULL;
    
    if (node->op && node->arg1 && node->result && strcmp(node->arg1, node->result) == 0 && strcmp(node->op, "=") == 0) {
        free(node);
        return ;
    }
    
    node->no = pc++;
    emits[emit_count++] = node;
}
void print_emit()
{
    for (int i = 0; i < emit_count; i++) {
        struct emit_node *tmp = emits[i];
        printf("%d: (%s, %s, %s, %s)\n", tmp->no, tmp->op ? tmp->op : "_", 
                tmp->arg1 ? tmp->arg1 : "_", 
                tmp->arg2 ? tmp->arg2 : "_",
                tmp->result ? tmp->result : "_");
    }
}
static inline void put_digital(int val, long address)
{
    data[address - 0xffff] = val;
}

static inline void put_paras_data(int val, long address)
{
    paras_data[address - 0xffffff] = val;
}

static int if_helper(struct AST_node_state_if *if_node, char *scope, long addr)
{
    struct AST_node_expr *left, *right;
    char *op = if_node->condition->op;
    left = if_node->condition->left;
    right = if_node->condition->right;
    char *tmp1 = new_temp();
    char *tmp2 = new_temp();
    walk_expr(left, scope, addr, tmp1);
    walk_expr(right, scope, addr, tmp2);
    jmp_emit_helper(tmp1, tmp2, op);
    walk_stmt(if_node->if_body, scope, addr);
    if (if_node->else_body) {
        emit("j", NULL, NULL, "todo");
        backfill();
        walk_stmt(if_node->else_body, scope, addr);
        backfill();
    } else {
        backfill();
    }
    return 0;
}
static int for_helper(struct AST_node_state_for *for_node, char *scope, long addr)
{
    struct AST_node_state_dec *init = for_node->init;
    char *tmp = new_temp();
    walk_expr(init->init_val, scope, addr, tmp);
    emit("=", tmp, NULL, init->var_name);
    insert_symbol(symbol_t, init->var_name, (char *)var_type_str[init->var_type], scope, addr);
    struct AST_node_condition *cond = for_node->cond;
    char *tmp1 = new_temp();
    char *tmp2 = new_temp();
    int record = pc;
    walk_expr(cond->left, scope, addr, tmp1);
    walk_expr(cond->right, scope, addr, tmp2);
    jmp_emit_helper(tmp1, tmp2, cond->op);
    struct AST_node_stmt *stmt = for_node->body;
    walk_stmt(stmt, scope, addr);
    struct AST_node_state_let *update = for_node->update;
    char *tmp3 = new_temp();
    walk_expr(update->var_expr, scope, addr, tmp3);
    emit("=", tmp3, NULL, update->var_name);
    char buf[16];
    memset(buf, 0, 16);
    sprintf(buf, "%d", record);
    emit("j", NULL, NULL, buf); 
    backfill();
    return 0;
}

static int return_helper(struct AST_node_state_return *return_node, char *scope, long addr)
{
    char *tmp = new_temp();
    walk_expr(return_node->ret_val, scope, addr, tmp);
    emit("=", tmp, NULL, "a7");
    emit("ret", NULL, NULL, NULL);
    return 0;
}

static inline void jmp_emit_helper(char *left_buf, char *right_buf, char *op)
{
        if (strcmp(op, ">") == 0) {
            emit("j<=", left_buf, right_buf, "todo"); 
        } else if (strcmp(op, "<") == 0) {
            emit("j>=", left_buf, right_buf, "todo"); 
        } else if (strcmp(op, ">=") == 0) {
            emit("j<", left_buf, right_buf, "todo"); 
        } else if (strcmp(op, "<=") == 0) {
            emit("j>", left_buf, right_buf, "todo"); 
        } else if (strcmp(op, "==") == 0) {
            emit("j!=", left_buf, right_buf, "todo"); 
        } else if (strcmp(op, "!=") == 0) {
            emit("j==", left_buf, right_buf, "todo");     
        }
}
static inline void backfill()
{
    for (int i = emit_count - 3; i >= 0; i--) {
        if (strcmp(emits[i]->result, "todo") == 0) {
            char buf[16];
            memset(buf, 0, 16);
            sprintf(buf, "%d", pc);
            memset(emits[i]->result, 0, 4);
            strncpy(emits[i]->result, buf, 8);
            break;
        }
    }
}

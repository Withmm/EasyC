#include "include/ast.h"
#include "include/symbol.h"
#include <stdio.h>
#include <string.h>
char *strdup(const char *src);
symbol_table_entry *symbol_t[TABLE_SIZE];

int pc;
int data_p = 0xffff;
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
void emit(char *op, char *arg1, char *arg2, char *result);
void walk_program(struct AST_node_program* pro);
void walk_declation_list(struct AST_node_declaration_list* dec_list);
void walk_func_dec(struct AST_node_func_dec* func_dec);

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
                print_symbol_table(symbol_t);
            } else {
                emit("=", "0", NULL, tmp->var_name);
            }
        }else if (dec_list->declarations[i]->basis->type == FUNC_DEC) { // func_declaration
			struct AST_node_func_dec *func_dec = dec_list->declarations[i]->real_dec.func;
            insert_symbol(symbol_t, 
                    strdup(func_dec->func_name), 
                    strdup(func_type_str[func_dec->func_type]), 
                        "nothing", 
                        pc);
			walk_func_dec(func_dec);				
		}
    }
}

void walk_func_dec(struct AST_node_func_dec* func_dec)
{
    (void)func_dec;
}
void walk_program(struct AST_node_program* pro) {
    walk_declation_list(pro->dec);
}

void handler_ast(struct AST_node_program* pro) {
    init_symbol_table(symbol_t, TABLE_SIZE);
    walk_program(pro);
    return;
}

void emit(char* op, char* arg1, char* arg2, char* result) {
    printf("(%s, %s, %s, %s)", op, arg1 ? arg1 : "_", arg2 ? arg2 : "_", result ? result : "_");
	printf("->%d\n", pc);
    pc++;
}


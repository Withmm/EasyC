#ifndef AST_H_
#define AST_H_
#define EMITSMAX 1024
enum AST_node_type {PROGRAM, DECLARATION_LIST, VAR_DEC, FUNC_DEC, PARAMS, STATE, STMT};
enum func_type_enum {VOID = 0, CHAR, SHORT, INT, LONG};
enum var_type_enum {VCHAR = 0, VSHORT, VINT, VLONG};
enum state_type_enum {INIT, LET, IF, FOR, RETURN};
struct emit_node{
    int no;
    char *op;
    char *arg1;
    char *arg2;
    char *result;
};
struct AST_node {
    enum AST_node_type type;
    int line;
};

struct AST_node_program {
    struct AST_node *basis;
    struct AST_node_declaration_list *dec;
};

struct AST_node_declaration_list {
    struct AST_node *basis;
    struct AST_node_declarations **declarations;
    long dec_capacity;
    int count;
};

struct AST_node_declarations {
    struct AST_node *basis;
    union {
        struct AST_node_var_dec_only *var;
        struct AST_node_func_dec *func;
    }real_dec;
};

struct AST_node_var_dec_only {
    struct AST_node *basis;
    enum var_type_enum var_type;
    char *var_name;
    long init_val;
};

struct AST_node_func_dec {
    struct AST_node *basis;
    enum func_type_enum func_type;
    char *func_name;
    struct func_symbol *sym;
    struct AST_node_func_paras *params; 
    struct AST_node_stmt *stmt;
    long para_addr;
};

struct func_symbol {
    char **paras_name;
    long *type;
};

struct AST_node_func_paras {
    struct AST_node *basis;
    int paras_count;
    int paras_capacity;
    char **paras_name;
    enum var_type_enum *paras_type;
};

struct AST_node_stmt {
    struct AST_node *basis;
    long state_count;
    long state_capacity;
    struct AST_node_state **state;
    struct AST_node_func_dec *from;
};

struct AST_node_state {
    struct AST_node *basis; 
    enum state_type_enum state_type;
    union {
        struct AST_node_state_if *real_if;
        struct AST_node_state_for *real_for;
        struct AST_node_state_return *real_return;
        struct AST_node_state_dec *real_dec; // int x; || int x = 0;
        struct AST_node_state_let *real_let; // x = y;
    }real_state;
    struct AST_node_func_dec *from;
};

struct AST_node_state_if {
    struct AST_node *basis;
    struct AST_node_condition *condition;
    struct AST_node_stmt *if_body;
    struct AST_node_stmt *else_body;
    struct AST_node_func_dec *from;
};

struct AST_node_state_for {
    struct AST_node *basis;
    struct AST_node_state_dec *init;          // Initialization
    struct AST_node_condition *cond;    // Condition
    struct AST_node_state_let *update;       // Update expression
    struct AST_node_stmt *body;         // Loop body
    struct AST_node_func_dec *from;
};


struct AST_node_state_return {
    struct AST_node *basis;
    struct AST_node_expr *ret_val;
    struct AST_node_func_dec *from;
};

struct AST_node_state_let {
    struct AST_node *basis;
    char *var_name;
    struct AST_node_expr *var_expr;
    struct AST_node_func_dec *from;
};

struct AST_node_state_dec {
    struct AST_node *basis;
    enum var_type_enum var_type;
    struct AST_node_expr *init_val;
    char *var_name;
    struct AST_node_func_dec *from;
};

struct AST_node_expr {  //E
    struct AST_node *basis;
    int val;
    struct AST_node_expr_T *expr_T;
    struct AST_node_expr_ *expr_;
    struct AST_node_func_dec *from;
};

struct AST_node_expr_ { //E'
    struct AST_node *basis;
    char op;
    int val;
    struct AST_node_expr_T *expr_T;
    struct AST_node_expr_ *expr_;
    struct AST_node_func_dec *from;
};

struct AST_node_expr_T { //T
    struct AST_node *basis;
    int val;
    struct AST_node_expr_t *expr_t;
    struct AST_node_expr_T_ *expr_T_;
    struct AST_node_func_dec *from;
};
struct AST_node_expr_T_ { //T'
    struct AST_node *basis;
    char op;
    int val;
    struct AST_node_expr_t *expr_t;
    struct AST_node_expr_T_ *expr_T_;
    struct AST_node_func_dec *from;
};

struct AST_node_expr_t { //F
    struct AST_node *basis;
    enum { CONSTANT, VARIABLE, FUNCTION_CALL, PARENTHESIZED_EXPR } type;
    int val;
    union {
        int val; // For constants
        char *var_name; // For variables
        struct AST_node_func_call *func_call;
        struct AST_node_expr *expr; // For parenthesized expressions
    } data;
    struct AST_node_func_dec *from;
};

struct AST_node_func_call {
    struct AST_node *basis;
    char *func_name;              // Name of the function
    struct AST_node_expr **params; // Array of pointers to parameters (expressions)
    int params_count;             // Number of parameters
    int params_capacity;          // Capacity of the parameters array
    int ret_val;
};

struct AST_node_condition {
    struct AST_node *basis;
    char *op;    
    struct AST_node_expr *left;
    struct AST_node_expr *right;
};

void handler_ast(struct AST_node_program* program);
#endif

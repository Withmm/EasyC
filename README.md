# EasyC 编译器

此编译器是本人学习编译原理的过程中，遵循书上的步骤用纯C写出的编译器。目前仅能完成从原始C语言到四元组的生成，供编译原理初学者学习参考。



## 项目优势：

- 结构清晰，耦合度低
- 与课程内容联系紧密，充分利用课堂知识
- 未使用任何编译相关高级框架，仅使用基础C语法，适合初学者阅读



## 主要工作：

- 从原始字符串到token流
- 从token流到AST树
- 解析AST树，生成四元组
- 总行数2500行左右



## 项目目录：

- `build`  存放中间目标文件`*.o`
- `easyc` 存放生成的四元组
- `log` 存放日志（编译错误）
- `src` 存放Easyc编译器源文件
  - `lex.c` 完成原始字符串到token流的转化
  - `parser.c` 完成token流到AST树的构建
  - `ast.c` 解析AST树，生成四元组
  - `symbol.c  `哈希表数据结构的简单实现
  - `main.c` 主文件，控制整体逻辑
  - `include` 目录存放相关头文件
- `test` 存放部分测试文件
  - `max.c` 求两数最大值
  - `fibonacci.c` 求斐波那契数列第n项
  - `expr.c` 表达式解析
  - `complex.for.c` `for` 里嵌套`if`
  - `complex.if.c` `if`里嵌套`if`
  - `undefined_error1.c`  在`if`的判断条件里用没定义过的变量
  - `undefined_error2.c `更改未定义过的变量
  - `double_defined_error.c` 在函数内二次定义一个变量
  - `double_defined_global_error.c` 在全局中二次定义一个变量
  - `identifiler_error1.c` 使用不合法的变量名（右值）
  - `identifiler_error2.c`使用不合法的变量名（左值）	
  - `not_right.c` 随便写的一堆字母
  - `for_error.c` `for`循环未使用大括号

## 代码部分单独讲解



### lex.c

**核心代码**

```C

int lexer(char *file_lexeme, struct Token *maintoken)
{
    int ntoken = 0;
    char *pchar = file_lexeme;
    while(*pchar) {
        char first_char = *pchar;
        struct Token token;
        memset(token.lexeme, 0, 32);
        int shift = 0;
        //用双层while循环模拟状态机 first_char 决定了当前处于什么状态(operetor? identifiler? constant?)
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
                pchar++;
                break;
            } else {
                printf("unknown : %c\n", *pchar);
                pchar++;
                break;
            }
        }
        strcpy(maintoken[ntoken].lexeme, token.lexeme);
        maintoken[ntoken].ttype = token.ttype;
        if (isspace(maintoken[ntoken].lexeme[0]) || maintoken[ntoken].lexeme[0] == 0)
            ntoken--;
        ntoken++;
        if (*pchar == '\n' || *pchar == ' ' || *pchar == '\t') {
            pchar++;
        }
    }
	//对token流进行一定的处理，把Keyword具体化
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
        //将操作符与操作符数组进行一一对比， 确认是合法的操作符(类似"<<<"的是会被归类到Operator的，但其实它并不是合法操作符)
        if (maintoken[i].ttype == Operator && !(is_operator(maintoken[i].lexeme, loperator))) {
            maintoken[i].ttype = Unknown;
            fprintf(stderr, "error : cannot identify %s\n", maintoken[i].lexeme);
            continue;
        }
		// #include <xx.h>里的"<" 和 ">" 进行了重载。（但实际上本编译器目前并没有实现include define等预处理指令）
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

```



**主要数据结构**

```C
enum tokentype{Identifier = 0, Keyword, Constant, Operator, Delimiter, Comment, Preprocessor, Unknown, Char, Short, Int, Long, Void};

struct Token {	
	enum tokentype ttype;
	char lexeme[32];
};
```

比如 `int`的对应的`struct Token`就是

`ttype = Int`

`lexeme[32] = "int"`



### parser.c



这一部分的代码会复杂很多，我们在这个文件里要完成

- 错误报告

- 设计AST树的数据结构
- 遍历Token数组，生成AST树



- **错误处理函数**：

  - `errormsg`：用于打印错误消息并退出程序。

   ```C
   void errormsg(struct Token *token, const char *format, ...)
   {
       fprintf(stderr, "line %d -> ", token[curtoken].line);
       va_list args;
       va_start(args, format);
           vfprintf(stderr, format, args);
       va_end(args);
           exit(-1);
   }
   ```

- **Token和AST节点的声明**：
  
  - ```C
      struct Token {  
      	enum tokentype ttype;
          	char lexeme[32];
    };
      /* 仅展示部分AST节点 */
    
    /*函数声明或者变量声明（均在global域）*/
    struct AST_node_declarations {
        struct AST_node *basis;
        union {
            struct AST_node_var_dec_only *var;
            struct AST_node_func_dec *func;
        }real_dec;
    };
    
    /* 全局变量声明 */
    struct AST_node_var_dec_only {
        struct AST_node *basis;
        enum var_type_enum var_type;
        char *var_name;
        long init_val;
    };
    
    /* 函数声明 */
    struct AST_node_func_dec {
        struct AST_node *basis;
        enum func_type_enum func_type;
        char *func_name;
        struct func_symbol *sym;
        struct AST_node_func_paras *params; 
        struct AST_node_stmt *stmt;
        long para_addr;
    };
    
    /* 函数参数声明 */
    struct AST_node_func_paras {
        struct AST_node *basis;
        int paras_count;
        int paras_capacity; //动态扩容的参数
        char **paras_name;
        enum var_type_enum *paras_type;
    };
    // ......
    ```
    
    
  
-  **全局变量**

  - `maxtoken`  :  `token个数`
  - `curtoken`  :  `当前检测到的token`



**递归下降法核心代码**

- 主要技术点

  - 递归下降原理

  - 动态扩容（不足扩容一倍）




```C
/* 函数参数的AST节点生成 */
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
        errormsg("paras error: missing (\n");
    }

    curtoken++;
    // )
    if (strcmp(token[curtoken].lexeme, ")") == 0) {
        curtoken++;
        return params;
    }

    while (1) {
        if (params->paras_count >= params->paras_capacity) { //动态扩容
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
                errormsg("paras error: invalid type\n");
        }

        curtoken++;
        // Identifier
        if (token[curtoken].ttype != Identifier) {
            errormsg("paras error: missing identifier\n");
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
            errormsg("paras error: missing , or )\n");
        }
    }
    return params;
}
```




AST树生成有利于后续的四元组生成，而且降低项目耦合度， 生成AST树之后所有的处理对象就是AST树而不再是token流了。

### ast.c



**主要功能**

- 根据`AST树`生成四元组

**四元组数据结构**

```C
struct emit_node{
    int no;
    char *op;
    char *arg1;
    char *arg2;
    char *result;
};
```

**核心代码**

```C
/* 仅展示部分 */
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
char *new_temp() {
    char *temp = malloc(20);
    sprintf(temp, "t%d", temp_var_count++);
    return temp;
}
```



**ABI接口协议**

- `a0 - a6`是函数参数

-  `a7`是函数返回值



**参考资料**

[表达式LL(1)文法](https://www.omegaxyz.com/2018/12/21/ll1-recursiondown/)

[北京大学编译原理文档](https://pku-minic.github.io/online-doc/#/)



**代码规范参考**

[Linux内核代码风格](https://www.kernel.org/doc/html/v4.13/translations/zh_CN/coding-style.html)

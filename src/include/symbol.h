#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define TABLE_SIZE 100
typedef struct symbol_table_entry {
    char *name;                    
    char *type;                    
    char *scope;                   
    long address;                   
    struct symbol_table_entry *next;  
} symbol_table_entry;


unsigned int hash(char *name);

symbol_table_entry *create_symbol_table_entry(char *name, char *type, char *scope, long address);

void init_symbol_table(symbol_table_entry **table, int table_size);

void insert_symbol(symbol_table_entry **table, char *name, char *type, char *scope, long address);

symbol_table_entry *find_symbol(symbol_table_entry **table, char *name, char *scope);

void delete_symbol(symbol_table_entry **table, char *name);

void print_symbol_table(symbol_table_entry **table);

#endif // SYMBOL_TABLE_H


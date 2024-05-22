#include "include/symbol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
char *strdup(const char *src);

unsigned int hash(char *name) {
    unsigned int hash_value = 0;
    for (char *p = name; *p != '\0'; p++) {
        hash_value = (hash_value << 5) + *p;
    }
    return hash_value % TABLE_SIZE;
}

symbol_table_entry *create_symbol_table_entry(char *name, char *type, char *scope, int address) {
    symbol_table_entry *new_entry = (symbol_table_entry *)malloc(sizeof(symbol_table_entry));
    new_entry->name = strdup(name);
    new_entry->type = strdup(type);
    new_entry->scope = strdup(scope);
    new_entry->address = address;
    new_entry->next = NULL;
    return new_entry;
}

void insert_symbol(symbol_table_entry **symbol_table,char *name, char *type, char *scope, int address) {
    unsigned int index = hash(name);
    symbol_table_entry *new_entry = create_symbol_table_entry(name, type, scope, address);

    if (symbol_table[index] == NULL) {
        symbol_table[index] = new_entry;
    } else {
        symbol_table_entry *current = symbol_table[index];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_entry;
    }
}

symbol_table_entry *find_symbol(symbol_table_entry **symbol_table, char *name) {
    unsigned int index = hash(name);
    symbol_table_entry *current = symbol_table[index];
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void delete_symbol(symbol_table_entry **symbol_table, char *name) {
    unsigned int index = hash(name);
    symbol_table_entry *current = symbol_table[index];
    symbol_table_entry *previous = NULL;

    while (current != NULL && strcmp(current->name, name) != 0) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        return; 
    }

    if (previous == NULL) {
        symbol_table[index] = current->next;
    } else {
        previous->next = current->next;
    }

    free(current->name);
    free(current->type);
    free(current->scope);
    free(current);
}

void print_symbol_table(symbol_table_entry **symbol_table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        symbol_table_entry *current = symbol_table[i];
        while (current != NULL) {
            printf("Name: %s, Type: %s, Scope: %s, Address: %d\n", current->name, current->type, current->scope, current->address);
            current = current->next;
        }
    }
}


void init_symbol_table(symbol_table_entry **table, int table_size)
{
    for (int i = 0; i < table_size; i++) {
        table[i] = create_symbol_table_entry("nothing", "nothing", "nothing", 0);
    }
}


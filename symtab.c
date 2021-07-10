#include <stdlib.h>
#include <string.h>
#include "symtab.h"

Symbol* newSymbol(char* name, size_t value, bool is_entry, SymbolSection section) {
    Symbol* sym = malloc(sizeof(Symbol));

    size_t len = strlen(name) + 1;
    char* symbols_name = malloc(len);
    memset(symbols_name, '\0', len); 
    strcpy(symbols_name, name);

    sym->symbol = symbols_name;
    sym->value = value;
    sym->is_entry = is_entry;
    sym->section = section;

    return sym;
}

void Symbol_free(Symbol* self) {
    free(self->symbol);
    free(self);
}


bool SymbolTable_insert(SymbolTable* self, Symbol* opcode) {
    ListNode *node = NULL;

    if (self->head == NULL) { 
        self->head = newListNode(opcode);
        return OK;
    }

    if (self->exists(self, opcode->symbol) == true) {
        /* should not insert two opcodes with same symbol */
        return ERR_DUPLICATE_SYMBOL;
    }

    /* put our new opcode first in list */
    node = newListNode(opcode);
    node->next = self->head;
    self->head = node;

    return OK;
}

bool SymbolTable_exists(SymbolTable* self, char* symbol_name) {
    return (bool) (self->find(self, symbol_name) != NULL);
}

Symbol* SymbolTable_find(SymbolTable* self, char* symbol_name) {
    ListIterator *iterator = NULL;
    ListNode *node = NULL;
    bool found = false;

    if (self->head == NULL) {
        return NULL;
    }

    iterator = newListIterator(self->head);
    node = iterator->next(iterator);

    while(node != NULL) {
        Symbol *op = NULL;
        if (node->data == NULL) {
            continue;
        }

        op = node->data;
        if (op->symbol == NULL) {
            continue;
        }

        if (strcmp(op->symbol, symbol_name) == 0) {
            found = true;
            break;
        }

        node = iterator->next(iterator);
    }

    iterator->free(iterator);

    if (found == true) {
        return (Symbol*) node->data;
    }

    return NULL;
}

void SymbolTable_free(SymbolTable* self) {
    if (self->head != NULL) {
        ListIterator *iterator = newListIterator(self->head);
        ListNode *node = iterator->next(iterator);

        while(node != NULL) {
            Symbol *op = node->data;
            if (op != NULL) {
                op->free(op);
            }

            node = iterator->next(iterator);
        }

        iterator->free(iterator);
    }

    self->insert = NULL;
    self->exists = NULL;
    self->find = NULL;
    self->free = NULL;

    free(self);
}

SymbolTable* newSymbolTable() {
    SymbolTable* list = malloc(sizeof(SymbolTable));

    list->head = NULL;
    list->insert = SymbolTable_insert;
    list->exists = SymbolTable_exists;
    list->find = SymbolTable_find;
    list->free = SymbolTable_free;

    return list;
}

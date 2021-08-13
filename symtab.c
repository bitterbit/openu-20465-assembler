#include "symtab.h"
#include <stdlib.h>
#include <string.h>

void dumpSymbolTable(SymbolTable *symtab) {
    ListIterator *iter = newListIterator(symtab->head);
    ListNode *node = iter->next(iter);

    while (node != NULL) {
        Symbol *sym = node->data;
        printf("symbol: %s\n", sym->symbol);
        node = iter->next(iter);
    }

    iter->free(iter);
}

Symbol *newSymbol(char *name, size_t value, bool is_entry, bool is_external,
                  SymbolSection section) {
    Symbol *sym = malloc(sizeof(Symbol));

    size_t len = strlen(name) + 1;
    char *symbols_name = calloc(1, len);
    strcpy(symbols_name, name);

    sym->symbol = symbols_name;
    sym->value = value;
    sym->is_entry = is_entry;
    sym->is_external = is_external;
    sym->section = section;

    return sym;
}

void Symbol_free(Symbol *self) {
    free(self->symbol);
    free(self);
}

ErrorType SymbolTable_insert(SymbolTable *self, Symbol *sym) {
    ListNode *node = NULL;

    if (self->head == NULL) {
        self->head = newListNode(sym);
        return SUCCESS;
    }

    if (self->exists(self, sym->symbol) == true) {
        /* should not insert two symbols with same name */
        return ERR_DUPLICATE_SYMBOL;
    }

    /* put our new symbol first in list */
    node = newListNode(sym);
    node->next = self->head;
    self->head = node;

    return SUCCESS;
}

bool SymbolTable_exists(SymbolTable *self, char *symbol_name) {
    return (bool)(self->find(self, symbol_name) != NULL);
}

Symbol *SymbolTable_find(SymbolTable *self, char *symbol_name) {
    ListIterator *iterator = NULL;
    ListNode *node = NULL;
    bool found = false;

    if (self->head == NULL) {
        return NULL;
    }

    iterator = newListIterator(self->head);
    node = iterator->next(iterator);

    while (node != NULL) {
        Symbol *sym = NULL;
        if (node->data == NULL) {
            continue;
        }

        sym = node->data;
        if (sym->symbol == NULL) {
            continue;
        }

        if (strcmp(sym->symbol, symbol_name) == 0) {
            found = true;
            break;
        }

        node = iterator->next(iterator);
    }

    iterator->free(iterator);

    if (found == true) {
        return (Symbol *)node->data;
    }

    return NULL;
}

void SymbolTable_free(SymbolTable *self) {
    if (self->head != NULL) {
        ListIterator *iterator = newListIterator(self->head);
        ListNode *node = iterator->next(iterator);

        while (node != NULL) {
            Symbol *sym = node->data;
            if (sym != NULL) {
                sym->free(sym);
            }

            node = iterator->next(iterator);
        }

        node->free(node);
        iterator->free(iterator);
    }

    self->insert = NULL;
    self->exists = NULL;
    self->find = NULL;
    self->free = NULL;

    free(self);
}

SymbolTable *newSymbolTable() {
    SymbolTable *list = malloc(sizeof(SymbolTable));

    list->head = NULL;
    list->insert = SymbolTable_insert;
    list->exists = SymbolTable_exists;
    list->find = SymbolTable_find;
    list->free = SymbolTable_free;

    return list;
}

ErrorType SymbolManager_insertSymbol(SymbolManager *self, char *name,
                                     size_t value, bool is_extern,
                                     bool is_entry, SymbolSection section) {

    Symbol *sym = newSymbol(name, value, is_extern, is_entry, section);
    ErrorType err = self->symtab->insert(self->symtab, sym);

    if (err != SUCCESS) {
        sym->free(sym);
    }

    return err;
}

ErrorType SymbolManager_markSymEntry(SymbolManager *self, char *name) {
    Symbol *sym = self->symtab->find(self->symtab, name);

    if (sym == NULL) {
        return ERR_ENTRY_SYM_NOT_FOUND;
    }

    sym->is_entry = true;
    return SUCCESS;
}

Symbol *SymbolManager_useSymbol(SymbolManager *self, char *name) {
    /* TODO mark symbol as used */
    return self->symtab->find(self->symtab, name);
}

void SymbolManager_fixDataSymbolsOffset(SymbolManager *self, size_t offset) {
    Symbol *sym = NULL;
    ListNode *node = NULL;
    ListIterator *iterator = newListIterator(self->symtab->head);

    while ((node = iterator->next(iterator)) != NULL) {
        sym = node->data;

        if (sym == NULL) {
            continue;
        }

        if (sym->section != SymbolSection_Data || sym->is_external == true) {
            continue;
        }

        sym->value += offset;
    }

    iterator->free(iterator);
}

void SymbolManager_free(SymbolManager *self) {}

SymbolManager *newSymbolManager() {
    SymbolManager *syms = malloc(sizeof(SymbolManager));
    syms->symtab = newSymbolTable();
    syms->usedSymbolsHead = NULL;

    syms->insertSymbol = SymbolManager_insertSymbol;
    syms->markSymEntry = SymbolManager_markSymEntry;
    syms->useSymbol = SymbolManager_useSymbol;
    syms->fixDataSymbolsOffset = SymbolManager_fixDataSymbolsOffset;
    syms->writeExtFile = NULL;
    syms->writeEntFile = NULL;
    syms->free = SymbolManager_free; /* TODO */

    return syms;
}


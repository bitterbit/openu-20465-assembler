#include "symtab.h"
#include <stdlib.h>
#include <string.h>

void dumpSymbolTable(SymbolTable *symtab) {
    ListIterator *iter = newListIterator(symtab->head);
    ListNode *node = iter->next(iter);

    while (node != NULL) {
        Symbol *sym = node->data;
        printf("symbol: %s value %lu\n", sym->symbol, sym->value);
        node = iter->next(iter);
    }

    iter->free(iter);
}

void Symbol_free(Symbol *self) {
    free(self->symbol);

    if (self->dependent_offsets != NULL) {
        free(self->dependent_offsets);
    }

    free(self);
    /* memset(self, 0, sizeof(Symbol)); */
}

Symbol *newSymbol(char *name, size_t value, bool is_entry, bool is_external,
                  SymbolSection section) {
    Symbol *sym = malloc(sizeof(Symbol));
    char *symbols_name = calloc(1, strlen(name) + 1);

    if (sym == NULL || symbols_name == NULL) {
        return NULL;
    }

    strcpy(symbols_name, name);

    sym->symbol = symbols_name;
    sym->value = value;
    sym->is_entry = is_entry;
    sym->is_external = is_external;
    sym->section = section;
    sym->free = Symbol_free;

    return sym;
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
            node->free(node);
            if (sym != NULL) {
                sym->free(sym);
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

SymbolTable *newSymbolTable() {
    SymbolTable *list = malloc(sizeof(SymbolTable));

    if (list == NULL) {
        return list;
    }

    list->head = NULL;
    list->insert = SymbolTable_insert;
    list->exists = SymbolTable_exists;
    list->find = SymbolTable_find;
    list->free = SymbolTable_free;

    return list;
}

ErrorType SymbolManager_insertSymbol(SymbolManager *self, char *name,
                                     size_t value, bool is_extern,
                                     SymbolSection section) {

    Symbol *sym = newSymbol(name, value, false, is_extern, section);
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

    if (sym->is_external == true) {
        return ERR_SYMBOL_CANNOT_BE_ENTRY_AND_EXTERN;
    }

    sym->is_entry = true;
    return SUCCESS;
}

Symbol *SymbolManager_useSymbol(SymbolManager *self, char *name, size_t instruction_counter) {
    Symbol *sym = self->symtab->find(self->symtab, name);
    if (sym == NULL) {
        return NULL;
    }

    if (sym->dependent_offsets_count == 0) {
        sym->dependent_offsets_count = 0;
        /* TODO: handle malloc and realloc failures? */
        sym->dependent_offsets = malloc(sizeof(size_t));
    } else {
        sym->dependent_offsets = realloc(sym->dependent_offsets, sym->dependent_offsets_count + 1);
    }

    sym->dependent_offsets[sym->dependent_offsets_count] = instruction_counter;
    sym->dependent_offsets_count += 1;

    return sym;
}

void SymbolManager_fixDataSymbolsOffset(SymbolManager *self, const size_t offset) {
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

void SymbolManager_writeExtFile(SymbolManager *self, FILE* file) {
    ListNode *node = NULL;
    ListIterator *iter = newListIterator(self->symtab->head);

    while ((node = iter->next(iter)) != NULL) {
        int i;

        Symbol *sym = node->data;

        if (sym == NULL || sym->is_external == false) {
            continue;
        }

        for (i=0; i<sym->dependent_offsets_count; i++) {
            fprintf(file, "%s %04lu\n", sym->symbol, sym->dependent_offsets[i]);
        }
    }

    iter->free(iter);
}

void SymbolManager_writeEntFile(SymbolManager *self, FILE* file) {
    ListNode *node = NULL;
    ListIterator *iter = newListIterator(self->symtab->head);

    while ((node = iter->next(iter)) != NULL) {
        Symbol *sym = node->data;

        if (sym == NULL || sym->is_entry == false) {
            continue;
        }

        fprintf(file, "%s %04lu\n", sym->symbol, sym->value);
    }

    iter->free(iter);
}

void SymbolManager_free(SymbolManager *self) {
    self->symtab->free(self->symtab);
    memset(self, 0, sizeof(SymbolManager));
    free(self);
}

/* TODO: handle malloc failure */
SymbolManager *newSymbolManager() {
    SymbolManager *syms = malloc(sizeof(SymbolManager));

    /* TODO: check for memory errors */
    syms->symtab = newSymbolTable();

    syms->insertSymbol = SymbolManager_insertSymbol;
    syms->markSymEntry = SymbolManager_markSymEntry;
    syms->useSymbol = SymbolManager_useSymbol;
    syms->fixDataSymbolsOffset = SymbolManager_fixDataSymbolsOffset;
    syms->writeExtFile = SymbolManager_writeExtFile;
    syms->writeEntFile = SymbolManager_writeEntFile;
    syms->free = SymbolManager_free;

    return syms;
}

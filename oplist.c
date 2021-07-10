#include <stdlib.h>
#include <string.h>
#include "oplist.h"

Opcode* newOpcode(char* sym, size_t value, bool is_entry, SymbolSection section) {
    Opcode* op = malloc(sizeof(Opcode));

    size_t len = strlen(sym) + 1;
    char* symbols_name = malloc(len);
    memset(symbols_name, '\0', len); 
    strcpy(symbols_name, sym);

    op->symbol = symbols_name;
    op->value = value;
    op->is_entry = is_entry;
    op->section = section;

    return op;
}

void Opcode_free(Opcode* self) {
    free(self->symbol);
    free(self);
}


bool OpcodeList_insert(OpcodeList* self, Opcode* opcode) {
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

bool OpcodeList_exists(OpcodeList* self, char* symbol_name) {
    return (bool) (self->find(self, symbol_name) != NULL);
}

Opcode* OpcodeList_find(OpcodeList* self, char* symbol_name) {
    ListIterator *iterator = NULL;
    ListNode *node = NULL;
    bool found = false;

    if (self->head == NULL) {
        return NULL;
    }

    iterator = newListIterator(self->head);
    node = iterator->next(iterator);

    while(node != NULL) {
        Opcode *op = NULL;
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
        return (Opcode*) node->data;
    }

    return NULL;
}

void OpcodeList_free(OpcodeList* self) {
    if (self->head != NULL) {
        ListIterator *iterator = newListIterator(self->head);
        ListNode *node = iterator->next(iterator);

        while(node != NULL) {
            Opcode *op = node->data;
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

OpcodeList* newOpcodeList() {
    OpcodeList* list = malloc(sizeof(OpcodeList));

    list->head = NULL;
    list->insert = OpcodeList_insert;
    list->exists = OpcodeList_exists;
    list->find = OpcodeList_find;
    list->free = OpcodeList_free;

    return list;
}

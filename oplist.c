#include <stdlib.h>
#include <string.h>
#include "oplist.h"
#include "err.h"


bool OpcodeList_insert(OpcodeList* self, Opcode* opcode) {
    if (self->head == NULL) { 
        self->head = newListNode(opcode);
        return OK;
    }

    if (self->exists(self, opcode->sybmol) == true) {
        /* should not insert two opcodes with same symbol */
        return ERR_DUPLICATE_SYMBOL;
    }

    /* put our new opcode first in list */
    ListNode *node = newListNode(opcode);
    node->next = self->head;
    self->head = node;

    return OK;
}

bool OpcodeList_exists(OpcodeList* self, char* symbol_name) {
    return false;
}

Opcode* OpcodeList_find(OpcodeList* self, char* symbol_name) {
    if (self->head == NULL) {
        return NULL;
    }

    ListIterator *iterator = newListIterator(self->head);
    ListNode *next = iterator->next(iterator);

    while(next != NULL) {
        
        next = iterator->next(iterator);
    }

    free(iterator);

    return NULL;
}

void OpcodeList_free(OpcodeList* self) {
    if (self->head != NULL) {
        ListIterator *iterator = newListIterator(self->head);
        ListNode *next = iterator->next(iterator);

        while(next != NULL) {
            free(next->data);
            next = iterator->next(iterator);
        }

        free(iterator);
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

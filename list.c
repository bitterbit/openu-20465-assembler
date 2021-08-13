#include "list.h"
#include <stdio.h>



/* will free the node but not its data! */
void ListNode_free(ListNode *self) { 
    free(self); 
}

/* TODO: make sure all users check if pointer is null */
ListNode *newListNode(void *data) {
    ListNode *node = malloc(sizeof(ListNode));
    node->next = NULL;
    node->data = data;
    node->free = ListNode_free;
    return node;
}

void ListIterator_free(ListIterator *self) {
    self->head = NULL;
    self->next = NULL;
    free(self);
}

ListNode *ListIterator_next(ListIterator *self) {
    ListNode *node = self->head;
    if (node != NULL) {
        self->head = self->head->next;
    }

    return node;
}

/* TODO: doesn't return err */
ListIterator *newListIterator(ListNode *head) {
    ListIterator *iter = malloc(sizeof(ListIterator));
    iter->head = head;
    iter->next = ListIterator_next;
    iter->free = ListIterator_free;

    return iter;
}

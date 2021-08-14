#include "list.h"
#include <stdio.h>



/* will free the node but not its data! */
void ListNode_free(ListNode *self) { 
    free(self); 
}

/* TODO: actually handle memory error until main */
ListNode *newListNode(void *data) {
    ListNode *node = malloc(sizeof(ListNode));

    if (node == NULL) {
        return NULL;
    }

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

/* TODO: handle malloc failure*/
ListIterator *newListIterator(ListNode *head) {
    ListIterator *iter = malloc(sizeof(ListIterator));

    if (iter == NULL) {
        return NULL;
    }

    iter->head = head;
    iter->next = ListIterator_next;
    iter->free = ListIterator_free;

    return iter;
}

#include "list.h"
#include <stdio.h>

/* will free the node but not its data! */
void ListNode_free(ListNode *self) { free(self); }

/* Create a new List Node */
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

/* Free the list iterator */
void ListIterator_free(ListIterator *self) {
    self->head = NULL;
    self->next = NULL;
    free(self);
}

/* Free Get next node from the iterator */
ListNode *ListIterator_next(ListIterator *self) {
    ListNode *node = self->head;
    if (node != NULL) {
        self->head = self->head->next;
    }

    return node;
}

/* Create a new list iterator, given a list head */
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

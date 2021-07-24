#include "list.h"

ListNode* newListNode(void* data) {
    ListNode* node = malloc(sizeof(ListNode));
    node->next = NULL;
    node->data = data;
    return node;
}

void ListIterator_free(ListIterator* self) {
    /* TODO: can't we free all the nodes in this function? */
    self->head = NULL;
    self->next = NULL;
    free(self);
}

ListNode* ListIterator_next(ListIterator* self) {
    ListNode* node = self->head;
    if (node != NULL) {
        self->head = self->head->next;
    }

    return node;
}

ListIterator* newListIterator(ListNode *head) {
    ListIterator* iter = malloc(sizeof(ListIterator));
    iter->head = head;
    iter->next = ListIterator_next;
    iter->free = ListIterator_free;

    return NULL;
}

#include "line_queue.h"

void LineQueue_push(LineQueue *self, AssemblyLine *line) {
    /* printf("queue->push\n"); */
    ListNode *node = newListNode(line);

    /* add our item to the end of the list
     * if there is a tail, connect it to the new tail before "loosing" reference
     * to it
     * */
    if (self->tail != NULL) {
        self->tail->next = node;
    }

    self->tail = node;

    /* if no head yet, this is the first item */
    if (self->head == NULL) {
        self->head = node;
    }
}

AssemblyLine *LineQueue_pop(LineQueue *self) {
    /* printf("queue->pop\n"); */
    AssemblyLine *first_line;
    ListNode *next;

    if (self->head == NULL) {
        return NULL;
    }

    next = self->head->next;
    /* printf("next node: %p\n", next); */
    first_line = self->head->data;

    /* this will free only the node and not its data */
    self->head->free(self->head);
    self->head = next;

    return first_line;
}

void LineQueue_free(LineQueue *self) {
    ListIterator *iterator = newListIterator(self->head);
    ListNode *node = iterator->next(iterator);

    while (node != NULL) {
        AssemblyLine *line = node->data;
        if (line != NULL) {
            freeLine(line);
        }

        node->free(node);
    }

    iterator->free(iterator);
}

LineQueue *newLineQueue() {
    LineQueue *queue = malloc(sizeof(LineQueue));
    queue->head = NULL;
    queue->tail = NULL;
    queue->push = LineQueue_push;
    queue->pop = LineQueue_pop;
    queue->free = LineQueue_free;
    return queue;
}

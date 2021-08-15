#include "line_queue.h"

/* Push the the line queue */
void LineQueue_push(LineQueue *self, AssemblyLine *line) {
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

/* Pop the the line queue */
AssemblyLine *LineQueue_pop(LineQueue *self) {
    AssemblyLine *first_line;
    ListNode *next;

    if (self->head == NULL) {
        return NULL;
    }

    next = self->head->next;
    first_line = self->head->data;

    /* this will free only the node and not its data */
    self->head->free(self->head);
    self->head = next;

    return first_line;
}

/* Free the LineQueue and its contents */
void LineQueue_free(LineQueue *self) {
    AssemblyLine *line = self->pop(self);
    while (line != NULL) {
        freeLine(line);
        line = self->pop(self);
    }

    free(self);
}

/* Create a new lineQueue object */
LineQueue *newLineQueue() {
    LineQueue *queue = malloc(sizeof(LineQueue));

    if (queue == NULL) {
        return queue;
    }

    queue->head = NULL;
    queue->tail = NULL;
    queue->push = LineQueue_push;
    queue->pop = LineQueue_pop;
    queue->free = LineQueue_free;
    return queue;
}

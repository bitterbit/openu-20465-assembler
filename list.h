#ifndef _H_LIST_
#define _H_LIST_

#include <stdlib.h>

typedef struct ListNode ListNode;
struct ListNode {
    ListNode* next;
    void* data;
    /* free only the node and not its data */
    void (*free)(ListNode *self);
};


typedef struct ListIterator ListIterator;

/* iterator to make iterating over a linked list easier */
struct ListIterator {
    ListNode* head;
    ListNode* (*next)(ListIterator *self);
    void (*free)(ListIterator *self);
};


ListNode* newListNode(void* data);
ListIterator* newListIterator(ListNode *list);

#endif

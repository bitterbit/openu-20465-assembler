#ifndef _H_LIST_
#define _H_LIST_

#include <stdlib.h>

typedef struct ListNode ListNode;
struct ListNode {
    ListNode* next;
    void* data;
};


typedef struct ListIterator ListIterator;
struct ListIterator {
    ListNode* head;
    ListNode* (*next)(ListIterator *self);
    void (*free)(ListIterator *self);
};


ListNode* newListNode(void* data);
ListIterator* newListIterator(ListNode *list);

#endif

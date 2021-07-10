#include <stdlib.h>

struct ListNode {
    ListNode* next;
    void* data;
};

struct ListIterator {
    ListNode* head;
    ListNode* (*next)(ListIterator *self);
    void (*free)(ListIterator *self);
};

typedef ListNode ListNode;
typedef ListIterator ListIterator;

ListNode* newListNode(void* data);
ListIterator* newListIterator(ListNode *list);

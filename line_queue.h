#ifndef _H_INSTRUCTION_QUEUE_
#define _H_INSTRUCTION_QUEUE_

#include <stdio.h>

#include "bool.h"
#include "list.h"
#include "err.h"
#include "assembly_line.h"


typedef struct LineQueue LineQueue;

/* LineQueue can hold AsseblyLines, this can be used to store parsed lines for processing in a later stage
 * this queue is FIFO 
 * if any AssemblyLines are left in queue when deallocated they will be deallocated too
 * */
struct LineQueue {
    ListNode *head;
    ListNode *tail;
    void (*push)(LineQueue *self, AssemblyLine *asm_line);
    AssemblyLine* (*pop)(LineQueue *self);
    void (*free)(LineQueue *self);
};

LineQueue* newLineQueue();

#endif

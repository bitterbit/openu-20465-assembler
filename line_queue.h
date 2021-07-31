#ifndef _H_INSTRUCTION_QUEUE_
#define _H_INSTRUCTION_QUEUE_

#include <stdio.h>

#include "bool.h"
#include "list.h"
#include "err.h"
#include "assembly_line.h"


typedef struct LineQueue LineQueue;

struct LineQueue {
    ListNode *head;
    void (*push)(LineQueue *self, AssemblyLine *asm_line);
    AssemblyLine* (*pop)(LineQueue *self);
    void (*free)(LineQueue *self);
};

LineQueue* newLineQueue();

#endif

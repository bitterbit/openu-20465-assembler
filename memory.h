#ifndef _H_MEMORY_
#define _H_MEMORY_ 

#include <stdlib.h>
#include <stdio.h>

#include "err.h"
#include "instruction.h"

#define INSTRUCTION_COUNTER_INITIAL_VALUE 100

typedef struct Memory Memory;

struct Memory {
    size_t data_counter;
    size_t instruction_counter;

    ErrorType (*writeData)(Memory* self, unsigned char* data, size_t size);
    ErrorType (*writeCode)(Memory* self, Instruction* instruction);
    void (*free)(Memory* self);
};

Memory* newMemory();

#endif

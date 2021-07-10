#include "memory.h"

Error Memory_writeData(Memory* self, unsigned char* data, size_t size) {
    return OK;
}

Error Memory_writeCode(Memory* self, Instruction* instruction) {
    return OK;
}

void Memory_free(Memory* self) {
    self->data_counter = 0;
    self->instruction_counter = 0;
    self->writeData = NULL;
    self->writeCode = NULL;
    self->free = NULL;
    free(self);
}

Memory* newMemory() {
    Memory* memory = (Memory*) malloc(sizeof(Memory));
    memory->data_counter = 0;
    memory->instruction_counter = INSTRUCTION_COUNTER_INITIAL_VALUE;
    memory->writeData = Memory_writeData;
    memory->writeCode = Memory_writeCode;
    memory->free = Memory_free;
    return memory;
}

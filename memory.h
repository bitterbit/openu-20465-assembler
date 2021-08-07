#ifndef _H_MEMORY_
#define _H_MEMORY_

#include <stdio.h>
#include <stdlib.h>

#include "err.h"
#include "instruction.h"

#define INSTRUCTION_COUNTER_INITIAL_VALUE 100
#define DATA_COUNTER_INITIAL_VALUE 0

typedef struct ManagedArray ManagedArray;
struct ManagedArray {
  size_t size;
  size_t capacity;
  unsigned char *data;

  void (*append)(ManagedArray *self, unsigned char *data, size_t size);
  void (*writeToFile)(ManagedArray *self, FILE *file);
  void (*free)(ManagedArray *self);
};

#define MANAGED_ARRAY_MIN_CAPACITY 8

ManagedArray *newManagedArray();

typedef struct Memory Memory;

struct Memory {
  size_t data_counter;
  size_t instruction_counter;

  ManagedArray *data;
  ManagedArray *code;

  ErrorType (*writeData)(Memory *self, unsigned char *data, size_t size);
  ErrorType (*writeCode)(Memory *self, Instruction *instruction);
  ErrorType (*toFile)(Memory *self, FILE* file);
  void (*free)(Memory *self);
};

Memory *newMemory();

#endif

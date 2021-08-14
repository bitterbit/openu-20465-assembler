#ifndef _H_MEMORY_
#define _H_MEMORY_

#include <stdio.h>
#include <stdlib.h>

#include "err.h"
#include "instruction.h"

#define INSTRUCTION_COUNTER_INITIAL_VALUE 100

typedef struct ManagedArray ManagedArray;
/* ManagedArray is an variable sized array that can be dumped to file
 * using writeToFile */
struct ManagedArray {
  size_t size;
  size_t capacity;
  unsigned char *data;

  ErrorType (*append)(ManagedArray *self, unsigned char *data, size_t size);
  ErrorType (*appendUnsignedInt)(ManagedArray *self, unsigned int data);
  /* write the content of the array to disk in a hexdump like form */
  void (*writeToFile)(ManagedArray *self, FILE *file);
  void (*free)(ManagedArray *self);
};

#define MANAGED_ARRAY_MIN_CAPACITY 8

ManagedArray *newManagedArray();

typedef struct Memory Memory;

/* Memory is a abstraction of an object file, 
 * one can use writeData and writeCode to fill the object file
 * and when done use toFile to write the object file to disk */
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

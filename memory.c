#include "memory.h"
#include <string.h>

ErrorType Memory_writeData(Memory *self, unsigned char *data, size_t size) {
  self->data->append(self->data, data, size);
  return SUCCESS;
}

ErrorType Memory_writeCode(Memory *self, Instruction *instruction) {
  self->code->append(self->code, (unsigned char *)instruction,
                     sizeof(Instruction));
  return SUCCESS;
}

ErrorType Memory_writeToFile(Memory *self, FILE *file) {
  self->code->writeToFile(self->code, file);
  self->data->writeToFile(self->data, file);
  return SUCCESS;
}

void Memory_free(Memory *self) {
  self->code->free(self->code);
  self->data->free(self->data);
  self->code = NULL;
  self->data = NULL;

  self->data_counter = 0;
  self->instruction_counter = 0;
  self->writeData = NULL;
  self->writeCode = NULL;
  self->free = NULL;
  free(self);
}

Memory *newMemory() {
  Memory *memory = (Memory *)malloc(sizeof(Memory));

  memory->data_counter = DATA_COUNTER_INITIAL_VALUE;
  memory->instruction_counter = INSTRUCTION_COUNTER_INITIAL_VALUE;

  memory->data = newManagedArray();
  memory->code = newManagedArray();

  memory->writeData = Memory_writeData;
  memory->writeCode = Memory_writeCode;
  memory->free = Memory_free;
  return memory;
}

void ManagedArray_append(ManagedArray *self, unsigned char *data, size_t size) {
  size_t leftover_size;
  if (self->size > self->capacity) {
    /* TODO return error? */
    return;
  }

  leftover_size = self->capacity - self->size;
  if (size > leftover_size) {

    /* make sure we will have enough space for new data */
    size_t missing = size - leftover_size;
    while (self->capacity < missing) {
      self->capacity *= 2;
    }

    self->data = realloc(self->data, self->capacity);
  }

  memcpy(self->data + self->size, data, size);
}

void ManagedArray_writeToFile(ManagedArray *self, FILE *file) {
  fwrite(self->data, self->size, 1, file);
}

void ManagedArray_free(ManagedArray *self) {
  free(self->data);
  self->data = NULL;
  self->size = 0;
  self->capacity = 0;
  self->append = NULL;
  self->writeToFile = NULL;
  self->free = NULL;
  free(self);
}

ManagedArray *newManagedArray() {
  ManagedArray *arr = (ManagedArray *)malloc(sizeof(ManagedArray));

  arr->free = ManagedArray_free;
  arr->append = ManagedArray_append;
  arr->writeToFile = ManagedArray_writeToFile;

  arr->capacity = MANAGED_ARRAY_MIN_CAPACITY;
  arr->size = 0;
  arr->data = malloc(arr->capacity);
  return arr;
}

#include "memory.h"
#include "object_file.h"

#include <string.h>

ErrorType Memory_writeData(Memory *self, unsigned char *data, size_t size) {
    self->data_counter += size;
    return self->data->append(self->data, data, size);
}

ErrorType Memory_writeCode(Memory *self, Instruction *instruction) {
    ErrorType err;

    err = self->code->appendUnsignedInt(self->code, instruction->body.inst);
    /* TODO: this need to be done in the first stage, so code labels are
     * correctly registered */
    /* self->instruction_counter += INSTRUCTION_SIZE; */

    return err;
}

ErrorType Memory_toFile(Memory *self, FILE *file) {
    int i;
    ObjectFile *objFile =
        newObjectFile(file, INSTRUCTION_COUNTER_INITIAL_VALUE);

    if (objFile == NULL) {
        return ERR_OUT_OF_MEMEORY;
    }

    objFile->writeHeader(objFile, self->code->size, self->data->size);
    for (i = 0; i < self->code->size; i++) {
        objFile->writeByte(objFile, self->code->data[i]);
    }

    for (i = 0; i < self->data->size; i++) {
        objFile->writeByte(objFile, self->data->data[i]);
    }

    /* flush file so we wont miss last bytes */
    fwrite("\n", 1, 1, file);
    fflush(file);

    objFile->free(objFile);

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

    ManagedArray *data_array = newManagedArray();
    ManagedArray *code_array = newManagedArray();

    if (memory == NULL || data_array == NULL || code_array == NULL) {
        return NULL;
    }

    memory->data_counter = DATA_COUNTER_INITIAL_VALUE;
    memory->instruction_counter = INSTRUCTION_COUNTER_INITIAL_VALUE;

    memory->data = data_array;
    memory->code = code_array;

    memory->writeData = Memory_writeData;
    memory->writeCode = Memory_writeCode;
    memory->toFile = Memory_toFile;
    memory->free = Memory_free;
    return memory;
}

ErrorType ManagedArray_append(ManagedArray *self, unsigned char *data,
                              size_t size) {
    size_t leftover_size;
    void *dst;

    if (self->size > self->capacity) {
        return ERR_MEMORY_INVALID_STATE;
    }

    leftover_size = self->capacity - self->size;

    if (size > leftover_size) {
        /* make sure we will have enough space for new data */
        while (leftover_size < size) {
            self->capacity *= 2;
            leftover_size = self->capacity - self->size;
        }

        self->data = realloc(self->data, self->capacity);
    }

    /* TODO: make sure this breaks the program */
    if (self->data == NULL) {
        return ERR_OUT_OF_MEMEORY;
    }

    dst = self->data + self->size;
    memcpy(dst, data, size);

    self->size += size;

    return SUCCESS;
}

ErrorType ManagedArray_appendUnsignedInt(ManagedArray *self,
                                         unsigned int data) {
    return self->append(self, (unsigned char *)&data, sizeof(data));
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
    self->appendUnsignedInt = NULL;
    self->writeToFile = NULL;
    self->free = NULL;
    free(self);
}

ManagedArray *newManagedArray() {
    ManagedArray *arr = (ManagedArray *)malloc(sizeof(ManagedArray));

    unsigned char *arr_data = malloc(MANAGED_ARRAY_MIN_CAPACITY);

    if (arr == NULL || arr_data == NULL) {
        return NULL;
    }

    arr->free = ManagedArray_free;
    arr->append = ManagedArray_append;
    arr->appendUnsignedInt = ManagedArray_appendUnsignedInt;
    arr->writeToFile = ManagedArray_writeToFile;

    arr->capacity = MANAGED_ARRAY_MIN_CAPACITY;
    arr->size = 0;
    arr->data = arr_data;
    return arr;
}

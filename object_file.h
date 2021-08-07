#ifndef _H_OBJECT_FILE_
#define _H_OBJECT_FILE_

#include <stdio.h>
#include <stdlib.h>

#define BYTES_PER_LINE 4

typedef struct ObjectFile ObjectFile;
struct ObjectFile {
    FILE* file;
    size_t base_address;
    size_t address;
    void (*writeHeader)(ObjectFile *self, size_t code_size, size_t data_size);
    void (*writeByte)(ObjectFile *self, unsigned char byte);
    void (*free)(ObjectFile *self);
};

ObjectFile *newObjectFile(FILE* file, size_t base_address);

#endif

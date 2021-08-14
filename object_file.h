#ifndef _H_OBJECT_FILE_
#define _H_OBJECT_FILE_

#include <stdio.h>
#include <stdlib.h>

#define BYTES_PER_LINE 4

typedef struct ObjectFile ObjectFile;
/* ObjectFile is a utility to simplfy writing an .ob file.
 * One can use it as if it is writing raw data and the output will match the guides format (hexdump like) */
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

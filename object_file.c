#include "object_file.h"

/* Write the header of the object file */
void ObjectFile_writeHeader(ObjectFile *self, size_t code_size,
                            size_t data_size) {
    fprintf(self->file, "      %zu %zu", code_size, data_size);
}

/* Write a byte to the object file */
void ObjectFile_writeByte(ObjectFile *self, unsigned char byte) {
    int count = self->address - self->base_address;

    /* check if we need to start a new line */
    if (count % BYTES_PER_LINE == 0) {
        fprintf(self->file, "\n %04lu", self->address);
    }

    fprintf(self->file, " %02X", byte);
    self->address++;
}

/* Free and object file instance */
void ObjectFile_free(ObjectFile *self) {
    self->file = NULL;
    self->address = self->base_address = 0;
    self->writeByte = NULL;
    self->writeHeader = NULL;
    self->free = NULL;

    free(self);
}

/* Create an object file instance */
ObjectFile *newObjectFile(FILE *file, size_t base_address) {
    ObjectFile *objFile = (ObjectFile *)malloc(sizeof(ObjectFile));

    if (objFile == NULL) {
        return NULL;
    }

    objFile->file = file;

    objFile->base_address = base_address;
    objFile->address = base_address;

    objFile->writeHeader = ObjectFile_writeHeader;
    objFile->writeByte = ObjectFile_writeByte;
    objFile->free = ObjectFile_free;

    return objFile;
}

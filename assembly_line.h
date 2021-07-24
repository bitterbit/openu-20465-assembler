#ifndef _H_ASSEMBLY_LINE_
#define _H_ASSEMBLY_LINE_

#include <stdio.h>

#include "instruction.h"
#include "err.h"

#define COMMENT_CHAR ';'
#define MAX_LINE_LENGTH 80
#define BUFFER_SIZE (MAX_LINE_LENGTH + 1)

typedef enum {
    TypeData,
    TypeCode,
    TypeEntry,
    TypeEmpty
} LineType;

typedef enum {
    FlagExternDecleration  = 1 << 0,
    FlagSymbolDeclaration  = 1 << 1
} LineFlags;

/* TODO implement allocation and free functions */
typedef struct AssemblyLine AssemblyLine;
struct AssemblyLine {
    LineType type;
    LineFlags flags;

    char* label;
    char* opcode_name;
    char** args;
    size_t arg_count;
};


ErrorType parseLine(FILE *file, AssemblyLine *line);
unsigned char* decodeDataLine(AssemblyLine *line, size_t* out_size);

Instruction* decodeInstructionLine(AssemblyLine* line);

#endif

#ifndef _H_ASSEMBLY_LINE_
#define _H_ASSEMBLY_LINE_

#include <stdio.h>

#include "err.h"
#include "instruction.h"
#include "symtab.h"

#define COMMENT_CHAR ';'
#define MAX_LINE_LENGTH 80
#define BUFFER_SIZE (MAX_LINE_LENGTH + 1)

typedef enum {
    TypeData = 1,
    TypeCode,
    TypeEntry,
    TypeExtern,
    TypeEmpty
} LineType;

/* TODO: can just be bool? */
typedef enum { FlagSymbolDeclaration = 1 << 0 } LineFlags;

typedef struct DebugInfo DebugInfo;
struct DebugInfo {
    int line_number;
};

typedef struct AssemblyLine AssemblyLine;
struct AssemblyLine {
    LineType type;
    LineFlags flags;

    char label[32];
    char opcode_name[7]; /* Longest opcode is 6 chars long */
    char **args;
    size_t arg_count;
    int code_position;

    DebugInfo debug_info;
};

ErrorType parseLine(FILE *file, AssemblyLine *line);
ErrorType numberFromString(char *str, int *number, int number_of_bits);
unsigned char *decodeDataLine(AssemblyLine *line, size_t *out_size,
                              ErrorType *out_err);

ErrorType decodeInstructionLine(AssemblyLine *line, Instruction *inst,
                                SymbolManager *syms);

AssemblyLine *newLine();
void freeLine(AssemblyLine *line);
void dumpAssemblyLine(AssemblyLine *line);

#endif

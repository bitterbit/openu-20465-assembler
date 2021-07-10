/*
 * --- first pass
 *  + parse opecodes into queue
 *  + collect all labels
 *  + write data entries? 
 *
 * --- second pass
 *  + fix addresses of opcodes in queue that are unresolved (symbols)
 *  + resolve externals?
 *
 * --- end
 *  + dump to files
 *   + object file with `.ob` for machine code
 *   + textual files with "key value" lines
       + externals files with data about unresolved symbols that were marked as externals. `.ext`
       + entry files  `.ent` with all symbols that were marked as entry points
 *
 * for each file name in argv assemble the file
 *
 *
 *
 * concepts:
     * code section
     * memory - nice wrapper around buffer with cur pointer
     * instruction queue
         * Instruction struct
 *  
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "err.h"
#include "symtab.h"
#include "memory.h"
#include "instruction.h"

int main(int argc, char** argv) {
}

typedef enum {
    TypeData,
    TypeCode,
    TypeEntry
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

AssemblyLine* parseLine() {
    return NULL;
}

unsigned char* decodeDataLine(AssemblyLine *line, size_t* out_size) {
    return NULL;
}

Instruction* decodeInstructionLine(AssemblyLine* line) {
    return NULL;
}

#define MAX_SIZE 65535
#define INSTRUCTION_SIZE 4

void handle_assembly_file(char* path) {
    SymbolTable* symtab = newSymbolTable();
    Memory* memory = newMemory();
    Error err = OK;

    size_t code_size = 0;

    /* This is an outline of first pass */
    AssemblyLine *line = parseLine();

    while (line != NULL) {
        switch(line->type) {
            case TypeEntry:
                /* ignore on first pass */
                break;

            case TypeData:
                {
                    size_t size = 0;
                    unsigned char* data = NULL;

                    if (line->flags & FlagSymbolDeclaration && line->flags & FlagExternDecleration) {
                        Symbol* sym = newSymbol(line->label, 0, false, SymbolSection_Data);
                        err = symtab->insert(symtab, sym);
                    } 
                    else if (line->flags & FlagSymbolDeclaration) {
                        Symbol* sym = newSymbol(line->label, memory->data_counter, false, SymbolSection_Data);
                        err = symtab->insert(symtab, sym);
                    }

                    data = decodeDataLine(line, &size);
                    memory->writeData(memory, data, size);
                    free(data);
                }
                break;

            case TypeCode:
                if (line->flags & FlagSymbolDeclaration) {
                    Symbol* sym = newSymbol(line->label, memory->instruction_counter, false, SymbolSection_Code);
                    err = symtab->insert(symtab, sym);
                }

                /* count instructions here so we know what the final code size is before second pass */
                code_size += INSTRUCTION_SIZE;
                break;
        }

        line = parseLine();
    }

    /* end of first pass, start second pass */
    /* TODO maybe seek line back or just use stored parsed lines instead of reparsing */
    line = parseLine();
    while (line != NULL) {
        switch(line->type) {
            case TypeData:
                /* ignore data lines in the second pass */
                break;

            /* .entry SYM_NAME */
            case TypeEntry:
                {
                    Symbol* sym = NULL;
                    if (line->arg_count < 1) {
                        err = ERR_INVALID_ENTRY;
                        break;
                    }

                    sym = symtab->find(symtab, line->args[0]);
                    if (sym == NULL) {
                        err = ERR_ENTRY_SYM_NOT_FOUND;
                        break;
                    }

                    sym->is_entry = true;
                }
                
                break;

            case TypeCode:
                {
                    /* TODO check if instruction references .data section and calculate offset to it */
                    Instruction *inst = decodeInstructionLine(line);
                    memory->writeCode(memory, inst);
                }
                break;
        }

        line = parseLine();
    }

}

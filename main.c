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
#include "instruction.h"

int main(int argc, char** argv) {
}



typedef enum {
    TypeData,
    TypeCode,
    TypeEntry,
} LineType;

typedef enum {
    FlagExternDecleration  = 1 << 0,
    FlagSymbolDeclaration  = 1 << 1,
} LineFlags;

typedef struct AssemblyLine AssemblyLine;
struct AssemblyLine {
    LineType type;
    LineFlags flags;
    char* label;
    void* data;
    size_t data_size;
};

AssemblyLine* parseLine() {
    return NULL;
}

#define MAX_SIZE 65535
#define INSTRUCTION_SIZE 4
#define INSTRUCTION_COUNTER_INITIAL_VALUE 100

void handle_assembly_file(char* path) {
    unsigned char *output = malloc(MAX_SIZE);
    
    size_t instruction_counter = INSTRUCTION_COUNTER_INITIAL_VALUE;
    size_t data_counter = 0;
    Error err = OK;

    SymbolTable* symtab = newSymbolTable();

    /* This is an outline of first pass */
    AssemblyLine *line = parseLine();

    while (line != NULL) {
        switch(line->type) {
            case TypeEntry:
                /* ignore on first pass */
                break;

            case TypeData:
                if (line->flags & FlagSymbolDeclaration && line->flags & FlagExternDecleration) {
                    Symbol* sym = newSymbol(line->label, 0, false, SymbolSection_Data);
                    err = symtab->insert(symtab, sym);
                } 
                else if (line->flags & FlagSymbolDeclaration) {
                    Symbol* sym = newSymbol(line->label, data_counter, false, SymbolSection_Data);
                    err = symtab->insert(symtab, sym);
                }

                memcpy(output + data_counter, line->data, line->data_size);
                data_counter += line->data_size;
                break;

            case TypeCode:
                if (line->flags & FlagSymbolDeclaration) {
                    Symbol* sym = newSymbol(line->label, instruction_counter, false, SymbolSection_Code);
                    err = symtab->insert(symtab, sym);
                }

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

            case TypeEntry:
                {
                    Symbol* sym = symtab->find(symtab, line->data);
                    if (sym != NULL) {
                        sym->is_entry = true;
                    }
                }
                
                break;

            case TypeCode:
                {
                    Instruction* inst = line->data;
                    memcpy(output + instruction_counter, (void*) &inst->instruction, sizeof(inst->instruction));
                    instruction_counter += INSTRUCTION_SIZE;
                }
                break;
        }

        line = parseLine();
    }

}

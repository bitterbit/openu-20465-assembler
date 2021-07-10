..
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
    TypeData    = 1 << 0,
    TypeExtern  = 1 << 1,
    TypeEntry   = 1 << 2,
    TypeCode    = 1 << 3,
    TypeSymbol  = 1 << 4,
} LineType;

typedef struct AssemblyLine AssemblyLine;
struct AssemblyLine {
    LineType type;
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

    AssemblyLine *line = parseLine();

    while (line != NULL) {
        if (line->type & TypeData) {
            if (line->type & TypeSymbol) {
                Symbol* sym = newSymbol(line->label, data_counter, false, SymbolSection_Data);
                err = symtab->insert(symtab, sym);
            }

            /* TODO why does the symbol need to know if it is data or code? */
            memcpy(output + data_counter, line->data, line->data_size);
            data_counter += line->data_size;

            if (data_counter >= INSTRUCTION_COUNTER_INITIAL_VALUE) {
                /* TODO make this return error? maybe move this to Memory object? */
            }
        }

        if (line->type & TypeEntry) {
            /* do nothing, only in second pass */
        }

        if (line->type & TypeExtern) {
            /* TODO mark as extern and maybe remove SymbolSection_Data */
            Symbol* sym = newSymbol(line->label, 0, false, SymbolSection_Data);
            err = symtab->insert(symtab, sym);
        }

        if (line->type & TypeCode) {
            if (line->type & TypeSymbol) {
                Symbol* sym = newSymbol(line->label, instruction_counter, false, SymbolSection_Code);
                err = symtab->insert(symtab, sym);
            }

            Instruction inst;
            memcpy(output + instruction_counter, (void*) &inst.instruction, sizeof(inst.instruction));


            /* */
            instruction_counter += INSTRUCTION_SIZE;
        }

        /* if (line->type & */ 

        AssemblyLine *line = parseLine();
    }

}

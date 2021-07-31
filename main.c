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
#include "assembly_line.h"
#include "str_utils.h"

int main(int argc, char** argv) {
    /* TODO: do we need to catch all memory allocation errors? */
}

#define MAX_SIZE 65535
#define INSTRUCTION_SIZE 4

ErrorType handle_assembly_file(char* path) {
    SymbolTable* symtab = newSymbolTable();
    Memory* memory = newMemory();
    ErrorType err = SUCCESS;

    size_t code_size = 0;
    AssemblyLine line;

    /* TODO: remember to close file */
    FILE *file = openfile(path, &err);

    /* TODO: probably better to break out of this function with an error and print all errors at one point */
    if (err != SUCCESS){
        print_error(err);
    }

    /* This is an outline of first pass */
    err = parseLine(file, &line);

    while (err == SUCCESS) {
        switch(line.type) {
            case TypeEmpty:
                /* ignore on Empty or comment lines */
                break;

            case TypeEntry:
                /* ignore on first pass */
                break;

            case TypeData:
                {
                    size_t size = 0;
                    unsigned char* data = NULL;

                    if (line.flags & FlagSymbolDeclaration && line.flags & FlagExternDecleration) {
                        Symbol* sym = newSymbol(line.label, 0, false, SymbolSection_Data);
                        err = symtab->insert(symtab, sym);
                    } 
                    else if (line.flags & FlagSymbolDeclaration) {
                        Symbol* sym = newSymbol(line.label, memory->data_counter, false, SymbolSection_Data);
                        err = symtab->insert(symtab, sym);
                    }

                    data = decodeDataLine(&line, &size);
                    memory->writeData(memory, data, size);
                    free(data);
                }
                break;

            case TypeCode:
                if (line.flags & FlagSymbolDeclaration) {
                    Symbol* sym = newSymbol(line.label, memory->instruction_counter, false, SymbolSection_Code);
                    err = symtab->insert(symtab, sym);
                }

                /* count instructions here so we know what the final code size is before second pass */
                code_size += INSTRUCTION_SIZE;
                break;
        }
        clean_line(&line);
        err = parseLine(file, &line);
    }

    if (err != SUCCESS) {
        print_error(err);
        return err;
    }

    /* end of first pass, start second pass */
    /* TODO maybe seek line back or just use stored parsed lines instead of reparsing */

    err = parseLine(file, &line);
    while (err == SUCCESS) {
        switch(line.type) {
            case TypeEmpty:
                /* ignore on Empty or comment lines */
                break;

            case TypeData:
                /* ignore data lines in the second pass */
                break;

            /* .entry SYM_NAME */
            case TypeEntry:
                {
                    Symbol* sym = NULL;
                    if (line.arg_count < 1) {
                        err = ERR_INVALID_ENTRY;
                        break;
                    }

                    sym = symtab->find(symtab, line.args[0]);
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
                    Instruction *inst = decodeInstructionLine(&line);
                    memory->writeCode(memory, inst);
                }
                break;
        }
        clean_line(&line);
        err = parseLine(file, &line);
    }

    if (err != SUCCESS) {
        print_error(err);
        return err;
    }

    return err;
}

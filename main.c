#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "err.h"
#include "symtab.h"
#include "memory.h"
#include "instruction.h"
#include "line_queue.h"
#include "assembly_line.h"
#include "str_utils.h"


#define MAX_SIZE 65535
#define INSTRUCTION_SIZE 4

ErrorType handle_assembly_file(char* path) {
    SymbolTable* symtab = newSymbolTable();
    Memory* memory = newMemory();
    LineQueue* queue = newLineQueue();
    AssemblyLine *line;

    ErrorType err = SUCCESS;

    size_t code_size = 0;

    /* TODO: probably better to break out of this function with an error and print all errors at one point */
    FILE *file = openfile(path, &err);
    if (err != SUCCESS){
        print_error(err);
    }

    /* lines are allocated on heap and owned by the queue */
    line = newLine();
    err = parseLine(file, line);
    queue->push(queue, line);

    while (err == SUCCESS) {

        printf("### line ###\n \tcmd: %s\n\tlabel: %s\n\t#args %lu\n", line->opcode_name, line->label, line->arg_count);


        switch(line->type) {
            case TypeEmpty:
                /* ignore on Empty or comment lines */
                break;

            case TypeEntry:
                /* ignore on first pass */
                break;

            case TypeExtern:
                {
                    Symbol* sym = newSymbol(line->label, 0, false, SymbolSection_Data);
                    err = symtab->insert(symtab, sym);
                }
                break;

            case TypeData:
                {
                    size_t size = 0;
                    unsigned char* data = NULL;

                    if (line->flags & FlagSymbolDeclaration) {
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

        line = newLine();
        err = parseLine(file, line);
        queue->push(queue, line);
    }

    /* TODO: we will need to print each of the errors - and add the line number, so this will be moved */
    if (err != SUCCESS) {
        print_error(err);
        return err;
    }

    /* end of first pass, start second pass */

    line = queue->pop(queue);

    while (line != NULL && err == SUCCESS) {
        switch(line->type) {
            case TypeEmpty:
                /* ignore on Empty or comment lines */
                break;

            case TypeData:
                /* ignore data lines in the second pass */
                break;

            case TypeExtern:
                /* ignore extern declerations in the second pass */
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

        freeLine(line);
        line = queue->pop(queue);
    }

    /* ==== cleanup ==== */
    fclose(file);

    if (err != SUCCESS) {
        print_error(err);
        return err;
    }

    return err;
}

int main(int argc, char** argv) {
    int i;
    ErrorType err = SUCCESS;

    /* skip first argument as it is binary name */
    for (i=1; i<argc; i++) {
        char *fname = argv[i];
        printf("parsing file %s\n", fname);
        err = handle_assembly_file(fname);

        if (err != SUCCESS) {
            print_error(err);
        }
    }
}

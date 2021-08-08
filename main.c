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

void printError(ErrorType err, AssemblyLine *line) {
    print_error(err);
    dumpAssemblyLine(line);
}


/* TODO: we don't verify the semantics of entry and external lines? others? */

bool handle_assembly_file(char* path) {
    FILE* outfile;
    bool error_happened = false;
    SymbolTable* symtab = newSymbolTable();
    Memory* memory = newMemory();
    LineQueue* queue = newLineQueue();
    /* lines are allocated on heap and owned by the queue */
    AssemblyLine *line = newLine();
    Instruction inst;
    int line_counter = 1;

    ErrorType err = SUCCESS;

    size_t code_size = 0;

    FILE *file = openfile(path, &err);
    if (err != SUCCESS){
        printError(err, line);
        return err;
    }

    printf("starting stage 1\n");

    err = parseLine(file, line);
    queue->push(queue, line);


    /* TODO: very ugly, to account for first addition */
    memory->instruction_counter -= INSTRUCTION_SIZE;

    while (err != ERR_EOF) {
        switch(line->type) {
            case TypeEmpty:
                /* ignore on Empty or comment lines */
                break;

            case TypeEntry:
                /* ignore on first pass */
                break;

            case TypeExtern:
                {
                    /* The external label is the first arg */
                    Symbol* sym = newSymbol(line->args[0], 0, false, true, SymbolSection_Data);
                    err = symtab->insert(symtab, sym);
                }
                break;

            case TypeData:
                {
                    size_t size = 0;
                    unsigned char* data = NULL;

                    if (line->flags & FlagSymbolDeclaration) {
                        Symbol* sym = newSymbol(line->label, memory->data_counter, false, false, SymbolSection_Data);
                        err = symtab->insert(symtab, sym);
                    }

                    data = decodeDataLine(line, &size);
                    memory->writeData(memory, data, size);
                    free(data);
                }
                break;

            case TypeCode:
                /* TODO: ugly */
                memory->instruction_counter += INSTRUCTION_SIZE;

                /* count instructions here so we know what the final code size is before second pass */
                code_size += INSTRUCTION_SIZE;

                if (line->flags & FlagSymbolDeclaration) {
                    Symbol* sym = newSymbol(line->label, memory->instruction_counter, false, false, SymbolSection_Code);
                    err = symtab->insert(symtab, sym);
                }

                break;
        }

        line = newLine();
        line->debug_info.line_number = line_counter;
        err = parseLine(file, line);

        if (err != SUCCESS && err != ERR_EOF) {
            error_happened = true;
            printError(err, line);
        }

        queue->push(queue, line);
        line_counter++;
    }

    /* end of first pass, start second pass */
    printf("stage1 code_size=%lu\n", code_size);
    printf("starting stage 2\n");
    err = SUCCESS;
    
    /* TODO: ugly, move stages to functions? */
    if (!error_happened) {
        while ((line = queue->pop(queue))) {
            switch(line->type) {
                case TypeEmpty:
                case TypeData:
                case TypeExtern:
                    /* ignore empty, data, and extern lines in second pass */
                    err = SUCCESS;
                    break;

                /* .entry SYM_NAME */
                case TypeEntry:
                    {
                        Symbol* sym = NULL;
                        if (line->arg_count < 1) {
                            err = ERR_INVALID_ENTRY;
                            break;
                        }

                        /* The entry label is the first arg */
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
                        /* Clean inst */
                        memset(&inst, 0, sizeof(Instruction));
                        /* TODO check if instruction references .data section and calculate offset to it */
                        printf("\n__PRINTING_LINE__\n");
                        err = decodeInstructionLine(line, &inst, symtab, memory->instruction_counter);
                        dumpAssemblyLine(line);
                        printf("%02x", (inst.body.inst >> (8*0)) & 0xff);
                        printf(" %02x", (inst.body.inst >> (8*1)) & 0xff);
                        printf(" %02x", (inst.body.inst >> (8*2)) & 0xff);
                        printf(" %02x\n", (inst.body.inst >> (8*3)) & 0xff);
                        printf("__FINSIHED_FINISHED__\n");

                        memory->writeCode(memory, &inst);
                    }
                    break;
            }

            if (err != SUCCESS) {
                error_happened = true;
                printError(err, line);
            }

            freeLine(line);
        }
    }

    outfile = fopen("output.ob", "w");
    memory->toFile(memory, outfile);
    fclose(outfile);

    /* TODO clean up even if we stop after first pass */
    queue->free(queue);
    memory->free(memory);

    /* ==== cleanup ==== */
    fclose(file);

    return error_happened;
}

int main(int argc, char** argv) {
    int i;

    /* skip first argument as it is binary name */
    for (i=1; i<argc; i++) {
        char *fname = argv[i];
        printf("parsing file %s\n", fname);
        handle_assembly_file(fname);
    }
}

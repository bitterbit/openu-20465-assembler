#include "assembly_line.h"
#include "err.h"
#include "instruction.h"
#include "line_queue.h"
#include "memory.h"
#include "str_utils.h"
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 65535

void printError(ErrorType err, AssemblyLine *line) {
    print_error(err);
    dumpAssemblyLine(line);
}
/* TODO: check for memory leaks */
/* TODO: Compile and check on the openu ubuntu */
/* TODO: Verify all syscalls (memory, etc..) are done safely, checked for errors */
/* TODO: we don't verify the semantics of entry and external lines? are there
 * any others? */

void fixDataSymbols(SymbolTable *symtab, size_t offset) {
    Symbol *sym = NULL;
    ListNode *node = NULL;
    ListIterator *iterator = newListIterator(symtab->head);

    while ((node = iterator->next(iterator)) != NULL) {
        sym = node->data;

        if (sym == NULL) {
            continue;
        }

        if (sym->section != SymbolSection_Data || sym->is_external == true) {
            continue;
        }

        sym->value += offset;
    }
    iterator->free(iterator);
}

bool handle_assembly_file(char *path) {
    FILE *outfile;
    bool error_happened = false;
    SymbolTable *symtab = newSymbolTable();
    Memory *memory = newMemory();
    LineQueue *queue = newLineQueue();
    /* lines are allocated on heap and owned by the queue */
    AssemblyLine *line = newLine();
    Instruction inst;
    int line_counter = 1;

    ErrorType err = SUCCESS;

    size_t code_size = 0;

    FILE *file = openfile(path, &err);
    if (err != SUCCESS) {
        printError(err, line);
        return err;
    }

    printf("starting stage 1\n");

    err = parseLine(file, line);
    queue->push(queue, line);

    while (err != ERR_EOF) {
        switch (line->type) {
        case TypeEmpty:
            /* ignore on Empty or comment lines */
            break;

        case TypeEntry:
            /* ignore on first pass */
            break;

        case TypeExtern: {
            /* The external label is the first arg */
            Symbol *sym =
                newSymbol(line->args[0], 0, false, true, SymbolSection_Data);
            err = symtab->insert(symtab, sym);
        } break;

        case TypeData: {
            size_t size = 0;
            unsigned char *data = NULL;

            if (line->flags & FlagSymbolDeclaration) {
                Symbol *sym = newSymbol(line->label, memory->data_counter,
                                        false, false, SymbolSection_Data);
                err = symtab->insert(symtab, sym);
            }

            data = decodeDataLine(line, &size, &err);
            memory->writeData(memory, data, size);
            free(data);
        } break;

        case TypeCode:
            if (line->flags & FlagSymbolDeclaration) {
                Symbol *sym =
                    newSymbol(line->label, memory->instruction_counter, false,
                              false, SymbolSection_Code);
                err = symtab->insert(symtab, sym);
            }

            /* TODO: move to parse line? */
            line->code_position = memory->instruction_counter;

            /* count instructions here so we know what the final code size is
             * before second pass */
            code_size += INSTRUCTION_SIZE;
            memory->instruction_counter += INSTRUCTION_SIZE;

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

    fixDataSymbols(symtab, memory->instruction_counter);

    /* end of first pass, start second pass */
    printf("stage1 code_size=%lu\n", code_size);
    printf("starting stage 2\n");
    err = SUCCESS;

    /* TODO: ugly, move stages to functions? */
    if (!error_happened) {
        while ((line = queue->pop(queue))) {
            switch (line->type) {
            case TypeEmpty:
            case TypeData:
            case TypeExtern:
                /* ignore empty, data, and extern lines in second pass */
                err = SUCCESS;
                break;

            /* .entry SYM_NAME */
            case TypeEntry: {
                Symbol *sym = NULL;
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

            case TypeCode: {
                /* Clean inst */
                memset(&inst, 0, sizeof(Instruction));
                printf("\n__PRINTING_LINE__\n");
                err = decodeInstructionLine(line, &inst, symtab);
                dumpAssemblyLine(line);
                printf("%02x", (inst.body.inst >> (8 * 0)) & 0xff);
                printf(" %02x", (inst.body.inst >> (8 * 1)) & 0xff);
                printf(" %02x", (inst.body.inst >> (8 * 2)) & 0xff);
                printf(" %02x\n", (inst.body.inst >> (8 * 3)) & 0xff);
                printf("__FINSIHED_FINISHED__\n");

                memory->writeCode(memory, &inst);
            } break;
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

int main(int argc, char **argv) {
    int i;

    /* skip first argument as it is binary name */
    for (i = 1; i < argc; i++) {
        char *fname = argv[i];
        printf("parsing file %s\n", fname);
        handle_assembly_file(fname);
    }
}

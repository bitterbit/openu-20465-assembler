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
/* TODO: Check for memory leaks */
/* TODO: Compile and check on the openu ubuntu */
/* TODO: Verify all syscalls (memory, etc..) are done safely, checked for errors */
/* TODO: Verify the semantics of entry and external lines? are there any others? */ 
/* TODO: implement output table for external and entry symbols */


ErrorType saveOutput(char* name, Memory *memory, SymbolManager *syms) {
    char filename[MAX_FILENAME_LENGTH];
    FILE *outfile;

    if (strlen(name) + 4 > MAX_FILENAME_LENGTH) {
        return ERR_FILENAME_TOO_LONG;
    }

    memset(filename, 0, MAX_FILENAME_LENGTH);
    strcpy(filename, name);
    strcat(filename, ".ob");
    outfile = fopen(filename, "w");
    if (outfile == NULL) {
        return ERR_CREATING_OUTPUT_FILE;
    }
    memory->toFile(memory, outfile);
    fclose(outfile);


    memset(filename, 0, MAX_FILENAME_LENGTH);
    strcpy(filename, name);
    strcat(filename, ".ext");
    outfile = fopen(filename, "w");
    if (outfile == NULL) {
        return ERR_CREATING_OUTPUT_FILE;
    }
    syms->writeExtFile(syms, outfile);
    fclose(outfile);

    memset(filename, 0, MAX_FILENAME_LENGTH);
    strcpy(filename, name);
    strcat(filename, ".ent");
    outfile = fopen(filename, "w");
    if (outfile == NULL) {
        return ERR_CREATING_OUTPUT_FILE;
    }
    syms->writeEntFile(syms, outfile);
    fclose(outfile);

    return SUCCESS;
}

bool handle_assembly_file(char *path) {
    bool error_happened = false;

    SymbolManager *syms = newSymbolManager();

    Memory *memory = newMemory();
    LineQueue *queue = newLineQueue();
    /* lines are allocated on heap and owned by the queue */
    AssemblyLine *line = newLine();
    Instruction inst;
    int line_counter = 1;
    char *output_name = NULL;
    ErrorType err = SUCCESS;
    size_t instruction_counter = 0;

    FILE *file = openfile(path, &err);
    if (file == NULL) {
        err = FILE_OPEN_ERROR;
    }

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
            err = syms->insertSymbol(syms, line->args[0], 0, false, true, SymbolSection_Data);

        } break;

        case TypeData: {
            size_t size = 0;
            unsigned char *data = NULL;

            if (line->flags & FlagSymbolDeclaration) {
                err = syms->insertSymbol(syms, line->label, memory->data_counter, false, false, SymbolSection_Data);
            }

            data = decodeDataLine(line, &size, &err);
            memory->writeData(memory, data, size);
            free(data);
        } break;

        case TypeCode:
            if (line->flags & FlagSymbolDeclaration) {
                err = syms->insertSymbol(syms, line->label, instruction_counter, false, false, SymbolSection_Code);
            }

            /* TODO: move to parse line? */
            line->code_position = instruction_counter;

            /* count instructions here so we know what the final code size is
             * before second pass */
            instruction_counter += INSTRUCTION_SIZE;

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

    syms->fixDataSymbolsOffset(syms, instruction_counter);

    /* end of first pass, start second pass */
    printf("stage1 code_size=%lu\n", instruction_counter);
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
                if (line->arg_count < 1) {
                    err = ERR_INVALID_ENTRY;
                    break;
                }

                /* The entry label is the first arg */
                err = syms->markSymEntry(syms, line->args[0]);
            }

            break;

            case TypeCode: {
                /* Clean inst */
                memset(&inst, 0, sizeof(Instruction));
                printf("\n__PRINTING_LINE__\n");
                err = decodeInstructionLine(line, &inst, syms);
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

    output_name = toBasename(path);
    removeFileExtension(output_name);
    if (output_name == NULL) {
        return ERR_CREATING_OUTPUT_FILE;
    }

    saveOutput(output_name, memory, syms);

    /* TODO clean up even if we stop after first pass */
    queue->free(queue);
    memory->free(memory);
    syms->free(syms);

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

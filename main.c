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
/* TODO: Verify the semantics of entry and external lines? are there any others? */ 


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
    size_t size;
    int line_counter = 1;
    unsigned char *data = NULL;

    ErrorType err = SUCCESS;
    size_t instruction_counter = INSTRUCTION_COUNTER_INITIAL_VALUE;
    FILE *file = openfile(path, &err);
    char* output_name = NULL;

    if (syms == NULL || memory == NULL || queue == NULL || line == NULL) {
        print_error(ERR_OUT_OF_MEMEORY);
        return false;
    }

    if (err != SUCCESS) {
        printError(err, line);
        return err;
    }

    printf("starting stage 1\n");

    /* TODO: i think errors are not handled nicelyyyy */
    err = parseLine(file, line);
    queue->push(queue, line);

    /* TODO: check for memory errors - exit if they happen */
    while (err != ERR_EOF) {
        switch (line->type) {
        case TypeEmpty:
            /* ignore on Empty or comment lines */
            break;

        case TypeEntry:
            if (line->arg_count != 1) {
                err = ERR_INVALID_ENTRY;
            }
            break;

        case TypeExtern: {
            printf("extern arg_count: %lu\n", line->arg_count);
            if (line->arg_count == 1) {
                err = syms->insertSymbol(syms, line->args[0], 0, false, true, SymbolSection_Data);
            }
            else {
                err = ERR_INVALID_EXTERNAL_LABEL_REFERENCE;
            }

        } break;

        case TypeData:
            size = 0;
            data = NULL;

            if (line->flags & FlagSymbolDeclaration) {
                err = syms->insertSymbol(syms, line->label, memory->data_counter, false, false, SymbolSection_Data);
            }

            if (err != SUCCESS) {
                break;
            }

            data = decodeDataLine(line, &size, &err);
            if (err != SUCCESS) {
                break;
            }

            err = memory->writeData(memory, data, size);
            free(data);
            break;

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

        if (err != SUCCESS && err != ERR_EOF) {
            error_happened = true;
            printError(err, line);
        }


        /* TODO: check for memory error */
        line = newLine();
        if (line == NULL) {
            err = ERR_OUT_OF_MEMEORY;
            error_happened = true;
            break;
        }

        line->debug_info.line_number = line_counter;
        err = parseLine(file, line);

        if (err != SUCCESS && err != ERR_EOF) {
            error_happened = true;
            printError(err, line);
        }

        queue->push(queue, line);
        line_counter++;

    }

    /* ensure first data and last code won't have the same address */
    instruction_counter += INSTRUCTION_SIZE;
    syms->fixDataSymbolsOffset(syms, instruction_counter);

    /* end of first pass, start second pass */
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

            case TypeCode:
                /* Clean inst */
                memset(&inst, 0, sizeof(Instruction));
                err = decodeInstructionLine(line, &inst, syms);
                /* printf("\n__PRINTING_LINE__\n"); */
                /* dumpAssemblyLine(line); */
                /* printf("%02x", (inst.body.inst >> (8 * 0)) & 0xff); */
                /* printf(" %02x", (inst.body.inst >> (8 * 1)) & 0xff); */
                /* printf(" %02x", (inst.body.inst >> (8 * 2)) & 0xff); */
                /* printf(" %02x\n", (inst.body.inst >> (8 * 3)) & 0xff); */
                /* printf("__FINSIHED_FINISHED__\n"); */

                if (err == SUCCESS) {
                    err = memory->writeCode(memory, &inst);
                }
                break;
            }

            /* TODO: exit on memory error? */
            if (err != SUCCESS) {
                error_happened = true;
                printError(err, line);
            }

        }
        freeLine(line);
    }


    output_name = toBasename(path);
    removeFileExtension(output_name);
    if (output_name == NULL) {
        return ERR_CREATING_OUTPUT_FILE;
    }

    saveOutput(output_name, memory, syms);

    /* TODO clean up even if we stop after first pass */
    queue->free(queue); /* queue owns the lines and will free them on queue->free */
    memory->free(memory);
    syms->free(syms);

    /* ==== cleanup ==== */
    fclose(file);

    /* TODO: isn't this the opposite? we return if there was an error? if there
     * was a success? */
    return error_happened;
}

int main(int argc, char **argv) {
    int i;

    /* skip first argument as it is binary name */
    for (i = 1; i < argc; i++) {
        char *fname = argv[i];
        printf("parsing file %s\n", fname);
        /* TODO: exit completely if memory error occured */
        handle_assembly_file(fname);
    }
}

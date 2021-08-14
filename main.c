#include "assembly_line.h"
#include "err.h"
#include "instruction.h"
#include "line_queue.h"
#include "memory.h"
#include "str_utils.h"
#include "symtab.h"
#include "first_pass.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 65535

/* TODO: Check for memory leaks */
/* TODO: Compile and check on the openu ubuntu */
/* TODO: Verify the semantics of entry and external lines? are there any others?
 */

ErrorType saveOutput(char *name, Memory *memory, SymbolManager *syms) {
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


bool handleAssemblyFile(char *path) {
    bool error_happened = false;

    SymbolManager *syms = newSymbolManager();
    Memory *memory = newMemory();
    LineQueue *queue = newLineQueue(); /* lines are allocated on heap and owned by the queue */
    AssemblyLine *line;
    Instruction inst;
    ErrorType err = SUCCESS;
    size_t instruction_counter;
    FILE *file;
    char *output_name;

    if (syms == NULL || memory == NULL || queue == NULL) {
        print_error(ERR_OUT_OF_MEMEORY);
        return false;
    }

    line = NULL;
    output_name = NULL;
    instruction_counter = INSTRUCTION_COUNTER_INITIAL_VALUE;

    file = openfile(path, &err);
    if (err != SUCCESS) {
        print_error(err);
        return false;
    }

    printf("starting stage 1\n");
    firstPass(file, syms, memory, queue, &instruction_counter);

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
                printLineError(err, line);
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
    queue->free(
        queue); /* queue owns the lines and will free them on queue->free */
    memory->free(memory);
    syms->free(syms);

    /* ==== cleanup ==== */
    fclose(file);

    /* TODO: isn't this the opposite? we return if there was an error? if
     * there was a success? */
    return error_happened;
}

int main(int argc, char **argv) {
    int i;

    /* skip first argument as it is binary name */
    for (i = 1; i < argc; i++) {
        char *fname = argv[i];
        printf("parsing file %s\n", fname);
        /* TODO: exit completely if memory error occured */
        handleAssemblyFile(fname);
    }
}

#include "assembly_line.h"
#include "err.h"
#include "first_pass.h"
#include "instruction.h"
#include "line_queue.h"
#include "memory.h"
#include "second_pass.h"
#include "str_utils.h"
#include "symtab.h"

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

void cleanup(SymbolManager **p_syms, Memory **p_memory, LineQueue **p_queue) {
    SymbolManager *syms = *p_syms;
    Memory *memory = *p_memory;
    LineQueue *queue = *p_queue;

    queue->free(queue);
    memory->free(memory);
    syms->free(syms);

    *p_syms = NULL;
    *p_memory = NULL;
    *p_queue = NULL;
}

bool handleAssemblyFile(char *path) {
    ErrorType err = SUCCESS;

    /* lines are allocated on heap and owned by the queue */
    LineQueue *queue = newLineQueue();
    SymbolManager *syms = newSymbolManager();
    Memory *memory = newMemory();
    size_t instruction_counter;
    FILE *file;
    char *output_name;

    if (syms == NULL || memory == NULL || queue == NULL) {
        printErr(ERR_OUT_OF_MEMEORY);
        return false;
    }

    output_name = NULL;
    instruction_counter = INSTRUCTION_COUNTER_INITIAL_VALUE;

    file = openfile(path, &err);
    if (err != SUCCESS) {
        printErr(err);
        cleanup(&syms, &memory, &queue);
        return false;
    }

    printf("[!] first pass\n");
    if (firstPass(file, syms, memory, queue, &instruction_counter) == false) {
        cleanup(&syms, &memory, &queue);
        fclose(file);
        return false;
    }

    fclose(file);

    /* end of first pass, start second pass */
    printf("[!] second pass\n");
    if (secondPass(syms, memory, queue) == false) {
        cleanup(&syms, &memory, &queue);
        return false;
    }

    output_name = toBasename(path);
    removeFileExtension(output_name);
    if (output_name == NULL) {
        printErr(ERR_CREATING_OUTPUT_FILE);
        cleanup(&syms, &memory, &queue);
        return false;
    }

    err = saveOutput(output_name, memory, syms);
    if (err != SUCCESS) {
        printErr(err);
        cleanup(&syms, &memory, &queue);
        return false;
    }

    cleanup(&syms, &memory, &queue);

    return true;
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

    return 0;
}

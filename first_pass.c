#include "first_pass.h"

ErrorType firstPassHandleLine(AssemblyLine *line, SymbolManager *syms,
                              Memory *memory, size_t *instruction_counter) {
    ErrorType err = SUCCESS;
    void *data = NULL;
    size_t size = 0;

    switch (line->type) {
    case TypeEmpty:
        /* ignore on Empty or comment lines */
        break;

    case TypeEntry:
        if (line->arg_count != 1) {
            return ERR_INVALID_ENTRY;
        }
        break;

    case TypeExtern: {
        if (line->arg_count != 1) {
            return ERR_INVALID_EXTERNAL_LABEL_REFERENCE;
        }
        err = syms->insertSymbol(syms, line->args[0], 0, false, true,
                                 SymbolSection_Data);
    } break;

    case TypeData:
        if (line->flags & FlagSymbolDeclaration) {
            err = syms->insertSymbol(syms, line->label, memory->data_counter,
                                     false, false, SymbolSection_Data);
        }

        if (err != SUCCESS) {
            return err;
        }

        data = decodeDataLine(line, &size, &err);
        if (err != SUCCESS) {
            return err;
        }

        err = memory->writeData(memory, data, size);
        free(data);
        break;

    case TypeCode:
        if (line->flags & FlagSymbolDeclaration) {
            err = syms->insertSymbol(syms, line->label, *instruction_counter,
                                     false, false, SymbolSection_Code);
        }

        line->code_position = *instruction_counter;
        /* count instructions here so we know what the final code size is
         * before second pass */
        *instruction_counter += INSTRUCTION_SIZE;
        break;
    }

    return err;
}

bool firstPass(FILE* file, SymbolManager *syms, Memory *memory, LineQueue *queue, size_t *instruction_counter) {
    ErrorType err = SUCCESS;
    AssemblyLine *line = NULL;
    size_t line_counter = 0;
    bool errored = false;

    line = newLine();
    err = parseLine(file, line);
    queue->push(queue, line);

    while (err != ERR_EOF) {
        err = firstPassHandleLine(line, syms, memory, instruction_counter);
        if (err != SUCCESS && err != ERR_EOF) {
            printLineError(err, line);
            errored = true;
        }

        line = newLine();
        if (line == NULL) {
            err = ERR_OUT_OF_MEMEORY;
            errored = true;
            break;
        }

        /* TODO remove debug info struct */
        line->debug_info.line_number = line_counter;
        err = parseLine(file, line);

        if (err != SUCCESS && err != ERR_EOF) {
            printLineError(err, line);
            errored = true;
        }

        queue->push(queue, line);
        line_counter++;
    }

    return errored == false;
}

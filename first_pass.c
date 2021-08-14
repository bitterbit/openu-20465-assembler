#include "first_pass.h"
#include "str_utils.h"


/* Handle a specific line for the first pass */
ErrorType firstPassHandleLine(AssemblyLine *line, SymbolManager *syms,
                              Memory *memory, size_t *instruction_counter) {
    ErrorType err = SUCCESS;
    void *data = NULL;
    size_t size = 0;

    if (err != SUCCESS) {
        return err;
    }

    switch (line->type) {
    case TypeEmpty:
        /* ignore Empty and comment lines */
        break;

    case TypeEntry:
        if (line->arg_count != 1) {
            return ERR_INVALID_SYNTAX_ENTRY_DECLERATION;
        }
        break;

    case TypeExtern: {
        if (line->arg_count != 1) {
            return ERR_INVALID_SYNTAX_EXTERN_DECLERATION;
        }
        err = syms->insertSymbol(syms, line->args[0], 0, true,
                                 SymbolSection_Data);
    } break;

    case TypeData:
        if (line->flags & FlagSymbolDeclaration) {
            err = syms->insertSymbol(syms, line->label, memory->data_counter,
                                     false, SymbolSection_Data);
        }

        if (err != SUCCESS) {
            return err;
        }

        data = decodeDataLine(line, &size, &err);
        if (err != SUCCESS) {
            if (data != NULL) {
                free(data);
            }
            return err;
        }

        err = memory->writeData(memory, data, size);
        free(data);
        break;

    case TypeCode:
        if (line->flags & FlagSymbolDeclaration) {
            err = syms->insertSymbol(syms, line->label, *instruction_counter,
                                     false, SymbolSection_Code);
        }

        line->code_position = *instruction_counter;
        /* count instructions here so we know what the final code size is
         * before second pass */
        *instruction_counter += INSTRUCTION_SIZE;
        break;
    }

    return err;
}

/* Main first pass function, iterate on file, and start parsing every line */
bool firstPass(FILE *file, SymbolManager *syms, Memory *memory,
               LineQueue *queue, size_t *instruction_counter, ErrorType *err) {
    AssemblyLine *line = NULL;
    size_t line_counter = 1;
    bool errored = false;
    *err = SUCCESS;

    line = newLine();
    line->debug_info.line_number = line_counter++;
    if (line == NULL) {
        printErr(ERR_OUT_OF_MEMEORY);
        return false;
    }

    *err = parseLine(file, line);
    queue->push(queue, line);

    while (*err != ERR_EOF) {
        *err = firstPassHandleLine(line, syms, memory, instruction_counter);

        if (*err != SUCCESS && *err != ERR_EOF) {
            printLineError(*err, line);
            errored = true;
            /* If a memory error occured, exit immediately */
            if (*err == ERR_OUT_OF_MEMEORY) {
                break;
            }
        }

        line = newLine();
        if (line == NULL) {
            *err = ERR_OUT_OF_MEMEORY;
            errored = true;
            break;
        }

        line->debug_info.line_number = line_counter++;
        *err = parseLine(file, line);

        if (*err != SUCCESS && *err != ERR_EOF) {
            printLineError(*err, line);
            errored = true;
            /* If a memory error occured, exit immediately */
            if (*err == ERR_OUT_OF_MEMEORY) {
                break;
            }
        }

        queue->push(queue, line);
    }

    syms->fixDataSymbolsOffset(syms, *instruction_counter);

    return errored == false;
}

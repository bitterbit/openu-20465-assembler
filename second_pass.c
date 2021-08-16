#include "second_pass.h"

/* Main function for the second pass. finish parsing the assembly, and pop each
 * line from the queue */
bool secondPass(SymbolManager *syms, Memory *memory, LineQueue *queue,
                ErrorType *err) {
    AssemblyLine *line;
    Instruction inst;
    bool had_errord = false;
    *err = SUCCESS;

    while ((line = queue->pop(queue))) {
        switch (line->type) {
        case TypeEmpty:
        case TypeData:
        case TypeExtern:
            /* ignore empty, data, and extern lines in second pass */
            *err = SUCCESS;
            break;

        /* .entry SYM_NAME */
        case TypeEntry: {
            /* we mark symbol as entry on second pass to make sure this symbol
             * will be already loaded into the symbol table */
            if (line->arg_count < 1) {
                *err = ERR_INVALID_SYNTAX_ENTRY_DECLERATION;
                break;
            }

            /* The entry label is the first arg */
            *err = syms->markSymEntry(syms, line->args[0]);
        }

        break;

        case TypeCode:
            /* Clean inst */
            memset(&inst, 0, sizeof(Instruction));
            *err = decodeInstructionLine(line, &inst, syms);

            if (*err == SUCCESS) {
                *err = memory->writeCode(memory, &inst);
            }
            break;
        }

        if (*err != SUCCESS) {
            printLineError(*err, line);
            had_errord = true;
            if (*err == ERR_OUT_OF_MEMEORY) {
                /* If its a memory error, break out of the loop */
                freeLine(line);
                break;
            }
            /* For any other error, continue looping */
            *err = SUCCESS;
        }

        freeLine(line);
    }

    return had_errord == false; /* return true if was successful */
}

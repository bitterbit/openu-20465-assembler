#include "second_pass.h"

bool secondPass(SymbolManager *syms, Memory *memory, LineQueue *queue) {
    ErrorType err = SUCCESS;
    AssemblyLine *line;
    Instruction inst;
    bool had_errord = false;

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
            printLineError(err, line);
            had_errord = true;
            err = SUCCESS;
        }

        freeLine(line);
    }

    return had_errord == false; /* return true if was successful */
}

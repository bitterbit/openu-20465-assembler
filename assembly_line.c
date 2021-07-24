
#include <stdlib.h>

#include "assembly_line.h"
#include "str_utils.h"
#include "err.h"

static const AssemblyLine EmptyLineStruct;


ErrorType parseLine(FILE *file, AssemblyLine *line) {
    ErrorType err = SUCCESS;
    char buf[BUFFER_SIZE]=  {0};

    /* Clean the line before parsing a new line into it */
    *line = EmptyLineStruct;

    /* TODO: how should longer lines be handled? */
    err = readline(file, buf);

    if (err != SUCCESS){
        return err;
    }

    /* Handle empty and commented lines */
    if(check_for_empty_line(buf) == 0 || buf[0] == COMMENT_CHAR){
        /* TODO: this function returns an error, maybe i can set the type of the line to empty line? */
        line->type = TypeEmpty;
        return err;
    }

    /* TODO: start parsing */

    /* TODO: parse label */

    /* TODO: understand type of command */
    /* TODO: set flags */
    /* TODO: parse name */
    /* TODO: args and args count*/
    
    return err;
}

/* TODO: Implement  */
unsigned char* decodeDataLine(AssemblyLine *line, size_t* out_size) {
    return NULL;
}

/* TODO: Implement  */
Instruction* decodeInstructionLine(AssemblyLine* line) {
    return NULL;
}

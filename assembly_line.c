
#include <stdlib.h>
#include <ctype.h>

#include "assembly_line.h"
#include "str_utils.h"
#include "err.h"

static const AssemblyLine EmptyLineStruct;


ErrorType parseLabel(char *buf, AssemblyLine *line){
    char* label = seperate_string_by_token(&buf, ":");

    if (label == NULL || contains_space(label)){
        /* if token is not found, or there is space before it, Set label to empty string */
        line->label[0] = '\0';
        return SUCCESS;
    }

    if (is_reserved_keyword(label) || strlen(label) > 31 || !isalpha(*label)){
        return ERR_INVALID_LABEL;
    }

    strcpy(line->label, label);

    return SUCCESS;
}


ErrorType parseCommand(char *buf, AssemblyLine *line){
    ErrorType err = SUCCESS;
    char* command = seperate_string_by_token(&buf, " \t");
    
    /* TODO: what is FlagSymbolDeclaration? */
    /* TODO: handle empty line? */

    if (*command != '.') {
        if (is_code_opcode(command)){
            line->type = TypeCode;
            strcpy(line->opcode_name, command);
        }
        else {
            err = ERR_INVALID_COMMAND_NAME;
        }
    }
    else{
        command += 1;
        if (str_in_str_array(command, (char**)data_directive_commands, data_directive_commands_len)){
            line->type = TypeData;
            strcpy(line->opcode_name, command);
        }
        else if (str_in_str_array(command, (char**)entry_directive_commands, entry_directive_commands_len)){
            line->type = TypeEntry;
            line->flags |= FlagExternDecleration;
            strcpy(line->opcode_name, command);
        }
        else {
            err = ERR_INVALID_COMMAND_NAME;
        }
    }

    return err;
}

ErrorType parseArgs(char *buf, AssemblyLine *line){
    /* TODO: parse args by type of command */
    /* TODO: parse arg count where needed */
    ErrorType err = SUCCESS;



    return err;
}


ErrorType parseLine(FILE *file, AssemblyLine *line) {
    ErrorType err = SUCCESS;
    char buf[BUFFER_SIZE]=  {0};

    /* Clean the line before parsing a new line into it */
    *line = EmptyLineStruct;

    err = readline(file, buf);

    if (err != SUCCESS){
        return err;
    }

    remove_leading_and_trailing_spaces((char**)&buf);

    /* Handle empty and commented lines */
    if(check_for_empty_line(buf) == 0 || buf[0] == COMMENT_CHAR){
        /* TODO: this function returns an error, maybe i can set the type of the line to empty line? */
        line->type = TypeEmpty;
        return err;
    }

    err = parseLabel(buf, line);
    if (err != SUCCESS){
        return err;
    }

    /* Remove leading spaces before command type */
    remove_leading_and_trailing_spaces((char**)&buf);
    err = parseCommand(buf, line);

    if (err != SUCCESS){
        return err;
    }

    /* Remove leading spaces before args */
    remove_leading_and_trailing_spaces((char**)&buf);
    err = parseArgs(buf, line);

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

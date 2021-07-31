
#include <stdlib.h>
#include <ctype.h>

#include "assembly_line.h"
#include "str_utils.h"
#include "err.h"

static const AssemblyLine EmptyLineStruct;


ErrorType parseLabel(char *buf, AssemblyLine *line){
    char* label = seperate_string_by_token(&buf, ":");

    if (label == NULL){
        /* if token is not found, Set label to empty string */
        line->label[0] = '\0';
        return SUCCESS;
    }

    if (is_reserved_keyword(label) || strlen(label) > 31 || !isalpha(*label) || contains_space(label)){
        return ERR_INVALID_LABEL;
    }

    line->flags |= FlagSymbolDeclaration;
    strcpy(line->label, label);

    return SUCCESS;
}



/* TODO: can be replaced with sscanf? */
/* Check if a token is a valid number in the assembly spec */
bool is_number(char *token)
{
    if(token == NULL || *token == '\0') return false;
    /* Numbers can start with a sign, but never contain only a sign */
    if(*token == '+' || *token == '-')
    {
        token++;
        if(!isdigit(*token++)){
            return false;
        }
    }

    /* All other chars should represent digits */
    while(*token != '\0')
    {
        if(!isdigit(*token++)){
            return false;
        }
    }
    return true;
}

bool is_register(char *token)
{
    int number;
    int sscanf_success;

    if ((*token++) != '$'){
        return false;
    }

    sscanf_success = sscanf(token, "%d", number);

    if (sscanf_success == 0){
        return false;
    }

    /* TODO: bug with trailing zeroes */
    if (number < 0 || number > 32){
        return false;
    }
    return true;
}



ErrorType parseCommand(char *buf, AssemblyLine *line){
    ErrorType err = SUCCESS;
    char* command = seperate_string_by_token(&buf, " \t");
    
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
        command++;
        if (str_in_str_array(command, (char**)data_directive_commands, data_directive_commands_len)){
            line->type = TypeData;
            strcpy(line->opcode_name, command);
        }
        /* TODO: in both of these types - verify label is empty */
        else if (str_in_str_array(command, (char**)entry_directive_commands, entry_directive_commands_len)){
            line->type = TypeEntry;
            strcpy(line->opcode_name, command);
        }
        else if (str_in_str_array(command, (char**)extern_directive_commands, extern_directive_commands_len)){
            line->type = TypeExtern;
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


    /* TODO: oh noez - a comma can also be inside a token*/
    ErrorType err = SUCCESS;
    line->arg_count = 0;
    line->args = NULL;

    if(check_for_empty_line(buf) == 0){
        return err;
    }




    return err;
}

AssemblyLine* newLine() {
    AssemblyLine *line = malloc(sizeof(AssemblyLine));
    return line;
}

void freeLine(AssemblyLine *line) {
    /* Clean a line and release the args pointers */
    int i;
    for (i=0; i < line->arg_count; i++){
        free(line->args[i]);
    }

    free(line);
}

/**
 * Fill AssemblyLine struct with data describing a single assembly line.
 * The caller is responsible for allocating and clearing.
 * Returns error if applicable
 */
ErrorType parseLine(FILE *file, AssemblyLine *line) {
    ErrorType err = SUCCESS;
    char buf[BUFFER_SIZE]=  {0};

    /* Clean the line before parsing a new line into it */
    /* TODO: need to free the args pointers (use the argcount) */
    *line = EmptyLineStruct;

    err = readline(file, buf);

    if (err != SUCCESS){
        return err;
    }

    remove_leading_and_trailing_spaces((char**)&buf);

    /* Handle empty and commented lines */
    if(check_for_empty_line(buf) == 0 || buf[0] == COMMENT_CHAR){
        line->type = TypeEmpty;
        return err;
    }

    err = parseLabel(buf, line);
    if (err != SUCCESS){
        return err;
    }

    /* If a line doesn't contain a command  - its invalid */
    if(check_for_empty_line(buf) == 0){
        return ERR_INVALID_COMMAND_FORMAT;
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

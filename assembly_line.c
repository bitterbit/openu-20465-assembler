
#include <stdlib.h>
#include <ctype.h>

#include "assembly_line.h"
#include "bit_utils.h"
#include "str_utils.h"
#include "err.h"


ErrorType parseLabel(char **buf, AssemblyLine *line){
    char *label;

    if (contains_char(*buf, ':') == false) {
        /* if token is not found, Set label to empty string */
        line->label[0] = '\0';
        return SUCCESS;
    }

    /* seperate_string_by_token always returns a string even if delimiter not found */
    label = seperate_string_by_token(buf, ":");
    printf("parseLabel label: %s \n", label);

    if (is_reserved_keyword(label) || strlen(label) > 31 || !isalpha(*label) || contains_space(label)){
        printf("invalid label: %s\n", label);
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


/* Return the register number if it is a valid register, -1 otherways */
int register_from_string(char *str)
{
    int number;
    int sscanf_success;

    if ((*str++) != '$'){
        return -1;
    }

    sscanf_success = sscanf(str, "%d", &number);

    if (sscanf_success == 0){
        return -1;
    }

    /* TODO: bug with trailing zeroes */
    if (number < 0 || number > 32){
        return -1;
    }
    return number;
}



ErrorType parseCommand(char **buf, AssemblyLine *line){
    ErrorType err = SUCCESS;

    /* split by space or tab */
    char* command = seperate_string_by_token(buf, " \t"); 
    printf("buf=%s buf after seperate_string_by_token \n", *buf);
    
    if (*command != '.') {
        if (is_code_opcode(command)){
            line->type = TypeCode;
            strcpy(line->opcode_name, command);
        }
        else {
            err = ERR_INVALID_COMMAND_NAME;
        }
    }
    else {
        command++;
        if (str_in_str_array(command, (char**)data_directive_commands, data_directive_commands_len)){
            line->type = TypeData;
            strcpy(line->opcode_name, command);
        }
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


/* Returns a new pointer to a token,
    caller is responsible for freeing */
char* handleToken(char **buf, ErrorType *err) {
    /* TODO: handle last token? */

    char *token;
    char *dyanimic_token = NULL;

    /* Remove leading spaces before arg */
    remove_leading_spaces(buf);
    printf("after removing spaces '%s'\n", *buf);

    /* Handle string tokens */
    if (**buf == '"') {
        printf("strings not supported\n");
        exit(100);
    }

    /* Handle non string tokens */
    else {
        token = seperate_string_by_token(buf, ",");
    }

    remove_trailing_spaces(&token);

    dyanimic_token = malloc(strlen(token)+1);
    strcpy(dyanimic_token, token);

    return dyanimic_token;

}

ErrorType parseArgs(char *buf, AssemblyLine *line){
    /* TODO: parse args by type of command */
    /* TODO: parse arg count where needed */
    char *token = NULL;


    /* TODO: oh noez - a comma can also be inside a token*/
    ErrorType err = SUCCESS;
    line->arg_count = 0;
    line->args = NULL;

    while (check_for_empty_line(buf) != 0) {
        /* token is heap allocated and should be freed when AssemblyLine is freed */
        token = handleToken(&buf, &err); 
        line->args = realloc(line->args, sizeof(void*) * (line->arg_count+1) );
        line->args[line->arg_count] = token;
        line->arg_count += 1;
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
    char buf[BUFFER_SIZE] = {0};
    char *buf_p = buf;
    err = readline(file, buf);

    if (err != SUCCESS){
        return err;
    }

    remove_leading_and_trailing_spaces((char**)&buf_p);

    /* Handle empty and commented lines */
    if(check_for_empty_line(buf_p) == 0 || buf_p[0] == COMMENT_CHAR){
        line->type = TypeEmpty;
        return err;
    }

    err = parseLabel(&buf_p, line);
    printf("line after parse label: %s\n", buf_p);

    if (err != SUCCESS){
        return err;
    }

    /* If a line doesn't contain a command  - its invalid */
    if(check_for_empty_line(buf_p) == 0){
        return ERR_INVALID_COMMAND_FORMAT;
    }

    /* Remove leading spaces before command type */
    remove_leading_spaces(&buf_p);
    err = parseCommand(&buf_p, line);
    printf("line after parse command: %s\n", buf_p);

    if (err != SUCCESS){
        return err;
    }

    err = parseArgs(buf_p, line);
    printf("line after parse args: %s\n", buf_p);

    return err;
}

/* TODO: Implement  */
unsigned char* decodeDataLine(AssemblyLine *line, size_t* out_size) {
    return NULL;
}


ErrorType number_from_string(char *str, int *immed, int number_of_bits){
    int sscanf_success;

    sscanf_success = sscanf(str, "%d", immed);

    if (sscanf_success == 0){
        return ERR_INVALID_NUMBER_TOKEN;
    }

    if (!number_fits_in_bits(*immed, number_of_bits)) {
        return ERR_INVALID_NUMBER_SIZE;
    }

    return SUCCESS;
}


ErrorType decodeIArithmetic(AssemblyLine* line, Instruction* inst) {
    int temp;
    ErrorType err = SUCCESS;

    /* First arg is a register */
    temp = register_from_string(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.i_inst.rs = temp;

    /* Second arg is a number that can be inserted into 16 bits */
    err = number_from_string(line->args[1], &temp, 16);

    if (err != SUCCESS){
        return err;
    }
    inst->instruction.i_inst.immed = temp;

    /* Third arg is a register */
    temp = register_from_string(line->args[2]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.i_inst.rt = temp;

    return SUCCESS;
}

ErrorType decodeIBranch(AssemblyLine* line, Instruction* inst) {
    int temp;

    /* First arg is a register */
    temp = register_from_string(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.i_inst.rs = temp;


    /* Second arg is a register */
    temp = register_from_string(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.i_inst.rt = temp;

    /* Third arg is a label */
    /* TODO: parse label:
       Need to get the distance to the label, and verify that it is a local label 
       The distance must fit in 16 bits*/

    return SUCCESS;
}


/* TODO: Duplicate of IArithmetic */
ErrorType decodeIMem(AssemblyLine* line, Instruction* inst) {
    int temp;
    ErrorType err = SUCCESS;

    /* First arg is a register */
    temp = register_from_string(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.i_inst.rs = temp;

    /* Second arg is a number that can be inserted into 16 bits */
    err = number_from_string(line->args[1], &temp, 16);

    if (err != SUCCESS){
        return err;
    }
    inst->instruction.i_inst.immed = temp;

    /* Third arg is a register */
    temp = register_from_string(line->args[2]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.i_inst.rt = temp;

    return SUCCESS;
}



ErrorType decodeIInstruction(AssemblyLine* line, Instruction* inst) {
    ErrorType err = SUCCESS;
    inst->type = I;

    /* TODO: decode opcode */

    /* Has 3 operands */
    if (line->arg_count != 3){
        return ERR_INVALID_CODE_INSTRUCTION;
    }

    /* TODO: If command is an arithmetic command */
    if (true) {
        err = decodeIArithmetic(line, inst);
    }

    /* TODO: If command is a branch command */
    else if (true) {
        err = decodeIBranch(line, inst);
    }

    else {
        err = decodeIMem(line, inst);
    }

    return err;
}


ErrorType decodeRArithmetic(AssemblyLine* line, Instruction* inst) {
    int temp;
    /* Has 3 operands */
    if (line->arg_count != 3){
        return ERR_INVALID_CODE_INSTRUCTION;
    }

    /* First arg is a register */
    temp = register_from_string(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.r_inst.rs = temp;

    /* Second arg is a register */
    temp = register_from_string(line->args[1]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.r_inst.rt = temp;

    /* Thirds arg is a register */
    temp = register_from_string(line->args[2]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.r_inst.rd = temp;

    return SUCCESS;
}

ErrorType decodeRMove(AssemblyLine* line, Instruction* inst) {
    int temp;
    /* Has 2 operands */
    if (line->arg_count != 2){
        return ERR_INVALID_CODE_INSTRUCTION;
    }

    /* First arg is a register */
    temp = register_from_string(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.r_inst.rd = temp;

    /* Second arg is a register */
    temp = register_from_string(line->args[1]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.r_inst.rs = temp;

    return SUCCESS;
}




ErrorType decodeRInstruction(AssemblyLine* line, Instruction* inst) {
    ErrorType err = SUCCESS;
    inst->type = R;

    /* All R instructions have opcode = 1 */
    /* TODO: the docs say its 1 and also say its zero 😢, pages 20-21 */
    inst->instruction.r_inst.opcode = 1;

    /* TODO: decode func */
    
    /* TODO: If command is an arithmetic command*/
    if (true) {
        err = decodeRArithmetic(line, inst);
    }

    else {
        err = decodeRMove(line, inst);
    }

    return err;
}


ErrorType decodeJInstruction(AssemblyLine* line, Instruction* inst) {
    int temp;
    inst->type = J;

    /* TODO: decode opcode */

    /* Use reg = 0 as default as only jumpt to register changes it to 1 */
    inst->instruction.j_inst.reg = 0;

    /* TODO: if command is stop */
    if (true) {
        /* Has 0 operands */
        if (line->arg_count != 0){
            return ERR_INVALID_CODE_INSTRUCTION;
        }
        return SUCCESS;
    }

    /* TODO: maybe split to functions */
    else {
        /* Has 1 operands */
        if (line->arg_count != 1){
            return ERR_INVALID_CODE_INSTRUCTION;
        }


        /* JUMP */
        if (true) {
            /* Try to decode a register */
            temp = register_from_string(line->args[0]);
            if (temp != -1) {
                inst->instruction.j_inst.address = temp;
                inst->instruction.j_inst.reg = 1;
            }
            else {
                /* Get label */

                /* TODO: if label is local, fill address with offset */

                /* TODO: if label is external - fill adress with zeros - no need to implement */
            }
        }

        /* LA */
        else  if (true) {
            /* TODO: same as second option in JUMP */
            /* Get label */

            /* TODO: if label is local, fill address with offset */

            /* TODO: if label is external - fill adress with zeros - no need to implement */
        }

        /* CALL */
        else {
            /* TODO: same as second option in JUMP */
            /* Get label */

            /* TODO: if label is local, fill address with offset */

            /* TODO: if label is external - fill adress with zeros - no need to implement */
        }
    }

    return SUCCESS;
}



ErrorType decodeInstructionLine(AssemblyLine* line, Instruction* inst) {
    ErrorType err = SUCCESS;

    /* is i instruction */
    if (str_in_str_array(line->opcode_name, (char**)i_commands, i_commands_len)) {
        err = decodeIInstruction(line, inst);
    }

    /* is r instruction */
    else if (str_in_str_array(line->opcode_name, (char**)r_commands, r_commands_len)) {
        err = decodeRInstruction(line, inst);
    }
    
    /* is j instruction */
    else if (str_in_str_array(line->opcode_name, (char**)j_commands, j_commands_len)) {
        err = decodeJInstruction(line, inst);
    }

    else {
        err = ERR_INVALID_INSTRUCTION_TYPE;
    }

    return err;
}

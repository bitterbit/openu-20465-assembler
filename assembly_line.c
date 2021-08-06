
#include <stdlib.h>
#include <ctype.h>

#include "assembly_line.h"
#include "bit_utils.h"
#include "str_utils.h"
#include "err.h"


ErrorType parseLabel(char **buf, AssemblyLine *line){
    char *label;

    if (containsChar(*buf, ':') == false) {
        /* if token is not found, Set label to empty string */
        line->label[0] = '\0';
        return SUCCESS;
    }

    /* splitString always returns a string even if delimiter not found */
    label = splitString(buf, ":");
    /* printf("parseLabel label: %s \n", label); */

    if (is_reserved_keyword(label) || strlen(label) > 31 || !isalpha(*label) || containsSpace(label)){
        /* printf("invalid label: %s\n", label); */
        return ERR_INVALID_LABEL;
    }

    line->flags |= FlagSymbolDeclaration;
    strcpy(line->label, label);

    return SUCCESS;
}



/* TODO: can be replaced with sscanf? */
/* Check if a token is a valid number in the assembly spec */
bool isNumber(char *token)
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
int registerFromString(char *str)
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
    char* command = splitString(buf, " \t"); 
    /* printf("buf=%s buf after splitString\n", *buf); */
    
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
        if (strArrayIncludes(command, (char**)data_directive_commands, data_directive_commands_len)){
            line->type = TypeData;
            strcpy(line->opcode_name, command);
        }
        else if (strArrayIncludes(command, (char**)entry_directive_commands, entry_directive_commands_len)){
            line->type = TypeEntry;
            strcpy(line->opcode_name, command);
        }
        else if (strArrayIncludes(command, (char**)extern_directive_commands, extern_directive_commands_len)){
            line->type = TypeExtern;
            strcpy(line->opcode_name, command);
        }
        else {
            err = ERR_INVALID_COMMAND_NAME;
        }
    }

    return err;
}


char* handleStringToken(char **buf, ErrorType *err) {
    char* token;
    splitString(buf, "\"");          /* skip the first quote char */
    token = splitString(buf, "\"");  /* take the value from current buf pointer until next quote */
    return token;
}

/* Returns a new pointer to a token,
    caller is responsible for freeing */
char* handleToken(char **buf, ErrorType *err) {
    /* TODO: handle last token? */

    char *token;
    char *dyanimic_token = NULL;

    /* Remove leading spaces before arg */
    removeLeadingSpaces(buf);
    /* printf("after removing spaces '%s'\n", *buf); */

    /* Handle string tokens */
    if (**buf == '"') {
        token = handleStringToken(buf, err);
    }

    /* Handle non string tokens */
    else {
        token = splitString(buf, ",");
    }

    removeTrailingSpaces(&token);

    dyanimic_token = malloc(strlen(token)+1);
    strcpy(dyanimic_token, token);

    return dyanimic_token;
}


ErrorType parseArgs(char *buf, AssemblyLine *line){
    /* TODO: parse args by type of command */
    char *token = NULL;


    /* TODO: oh noez - a comma can also be inside a token*/
    ErrorType err = SUCCESS;
    line->arg_count = 0;
    line->args = NULL;

    while (checkForEmptyLine(buf) != 0) {
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
    memset(line, '\0', sizeof(AssemblyLine));
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

    removeLeadingAndTrailingSpaces((char**)&buf_p);

    if (strlen(buf_p) == 0) {
        return ERR_EOF;
    }

    /* Handle empty and commented lines */
    if(checkForEmptyLine(buf_p) == 0 || buf_p[0] == COMMENT_CHAR){
        line->type = TypeEmpty;
        return err;
    }

    err = parseLabel(&buf_p, line);
    /* printf("line after parse label: %s\n", buf_p); */

    if (err != SUCCESS){
        return err;
    }

    /* If a line doesn't contain a command  - its invalid */
    if(checkForEmptyLine(buf_p) == 0){
        return ERR_INVALID_COMMAND_FORMAT;
    }

    /* Remove leading spaces before command type */
    removeLeadingSpaces(&buf_p);
    err = parseCommand(&buf_p, line);
    /* printf("line after parse command: %s\n", buf_p); */

    if (err != SUCCESS){
        return err;
    }

    err = parseArgs(buf_p, line);
    /* printf("line after parse args: %s\n", buf_p); */

    return err;
}

/* TODO: Implement  */


/*  
    Returns a buffer with the data that should be written to memory,
    Caller is responsible for freeing the buffer
*/
unsigned char* decodeDataLine(AssemblyLine *line, size_t* out_size) {
    int i, number, data_chunk_size;

    /* TODO: Return the err somehow */
    ErrorType err;
    unsigned char *buf = NULL;
    unsigned char *tmp;

    if (strncmp(ASCIZ, line->opcode_name, strlen(ASCIZ)) == 0) {
        if (line->arg_count != 1) {
            err = ERR_INVALID_DATA_INSTRUCTION;
            /* TODO: return err */
        }
        /* TODO: just write string, and make sure a null byte is added */
        /* TODO: also update out_size */
    }

    else {

        /* TODO: is that right? */
        if (line->arg_count == 0) {
            err = ERR_INVALID_DATA_INSTRUCTION;
            /* TODO: return err */
        }

        if (strncmp(DB, line->opcode_name, strlen(DB)) == 0) {
            data_chunk_size = 1;
        }
        else if (strncmp(DH, line->opcode_name, strlen(DH)) == 0) {
            data_chunk_size = 2;
        }
        else if (strncmp(DW, line->opcode_name, strlen(DW)) == 0) {
            data_chunk_size = 4;
        }

        *out_size = line->arg_count * data_chunk_size;
        
        /* Add 1 more byte for the null byte written by int_to_binary_string */
        buf = calloc(*out_size + 1 , 1);
        tmp = buf;

        for (i=0; i < line->arg_count; i++) {
            /* TODO: handle error */
            err = numberFromString(line->args[i], &number, data_chunk_size);

            /* TODO: If my impl has bugs, try itoa (copy source)*/
            write_binary_stream_to_buffer(number, data_chunk_size, tmp);
            tmp += data_chunk_size;
        }
    }

    return buf;
}


ErrorType numberFromString(char *str, int *number, int number_of_bits){
    int sscanf_success;

    sscanf_success = sscanf(str, "%d", number);

    if (sscanf_success == 0){
        return ERR_INVALID_NUMBER_TOKEN;
    }

    if (!number_fits_in_bits(*number, number_of_bits)) {
        return ERR_INVALID_NUMBER_SIZE;
    }

    return SUCCESS;
}


ErrorType decodeIArithmetic(AssemblyLine* line, Instruction* inst) {
    int temp;
    ErrorType err = SUCCESS;

    /* First arg is a register */
    temp = registerFromString(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.i_inst.rs = temp;

    /* Second arg is a number that can be inserted into 16 bits */
    err = numberFromString(line->args[1], &temp, 16);

    if (err != SUCCESS){
        return err;
    }
    inst->instruction.i_inst.immed = temp;

    /* Third arg is a register */
    temp = registerFromString(line->args[2]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.i_inst.rt = temp;

    return SUCCESS;
}

ErrorType decodeIBranch(AssemblyLine* line, Instruction* inst) {
    int temp;

    /* First arg is a register */
    temp = registerFromString(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.i_inst.rs = temp;


    /* Second arg is a register */
    temp = registerFromString(line->args[0]);
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


/* TODO: check after fix in definitions (page 23) */

/* TODO: Duplicate of IArithmetic */
ErrorType decodeIMem(AssemblyLine* line, Instruction* inst) {
    int temp;
    ErrorType err = SUCCESS;

    /* First arg is a register */
    temp = registerFromString(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.i_inst.rs = temp;

    /* Second arg is a number that can be inserted into 16 bits */
    err = numberFromString(line->args[1], &temp, 16);

    if (err != SUCCESS){
        return err;
    }
    inst->instruction.i_inst.immed = temp;

    /* Third arg is a register */
    temp = registerFromString(line->args[2]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.i_inst.rt = temp;

    return SUCCESS;
}



ErrorType decodeIInstruction(AssemblyLine* line, Instruction* inst) {
    ErrorType err = SUCCESS;
    inst->type = I;

    /* TODO: handle_error */
    inst->instruction.r_inst.opcode = command_to_opcode(line->opcode_name);

    /* Has 3 operands */
    if (line->arg_count != 3){
        return ERR_INVALID_CODE_INSTRUCTION;
    }

    switch(i_command_to_subtype(line->opcode_name)) {
        case IArithmetic:
            err = decodeIArithmetic(line, inst);
            break;

        case IBranch:
            err = decodeIBranch(line, inst);
            break;

        case IMem:
            err = decodeIMem(line, inst);
            break;
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
    temp = registerFromString(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.r_inst.rs = temp;

    /* Second arg is a register */
    temp = registerFromString(line->args[1]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.r_inst.rt = temp;

    /* Thirds arg is a register */
    temp = registerFromString(line->args[2]);
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
    temp = registerFromString(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.r_inst.rd = temp;

    /* Second arg is a register */
    temp = registerFromString(line->args[1]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->instruction.r_inst.rs = temp;

    return SUCCESS;
}




ErrorType decodeRInstruction(AssemblyLine* line, Instruction* inst) {
    ErrorType err = SUCCESS;
    inst->type = R;

    /* TODO: handle_error */
    inst->instruction.r_inst.opcode = command_to_opcode(line->opcode_name);

    /* TODO: handle_error */
    inst->instruction.r_inst.funct = r_command_to_func(line->opcode_name);

    
    switch(r_command_to_subtype(line->opcode_name)) {
        case RArithmetic:
            err = decodeRArithmetic(line, inst);
            break;

        case RMove:
            err = decodeRMove(line, inst);
            break;
    }

    return err;
}


ErrorType decodeJInstruction(AssemblyLine* line, Instruction* inst) {
    int registed_number;
    inst->type = J;

    /* TODO: handle_error */
    inst->instruction.r_inst.opcode = command_to_opcode(line->opcode_name);

    /* Use reg = 0 as default as only jump to register changes it to 1 */
    inst->instruction.j_inst.reg = 0;

    /* STOP */
    if (strncmp(line->opcode_name, STOP, strlen(STOP))) {
        /* Has 0 operands */
        if (line->arg_count != 0){
            return ERR_INVALID_CODE_INSTRUCTION;
        }
        return SUCCESS;
    }

    else {
        /* Has 1 operand */
        if (line->arg_count != 1){
            return ERR_INVALID_CODE_INSTRUCTION;
        }

        registed_number = registerFromString(line->args[0]);
        /* if JMP and first arg is a register */
        if (strncmp(line->opcode_name, JMP, strlen(JMP)) == 0 && registed_number != -1) {
            inst->instruction.j_inst.address = registed_number;
            inst->instruction.j_inst.reg = 1;
        }
         
         /* JMP with label, LA, CALL */
        else {
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
    if (is_i_command(line->opcode_name)) {
        err = decodeIInstruction(line, inst);
    }

    /* is r instruction */
    else if (is_r_command(line->opcode_name)) {
        err = decodeRInstruction(line, inst);
    }
    
    /* is j instruction */
    else if (is_j_command(line->opcode_name)) {
        err = decodeJInstruction(line, inst);
    }

    else {
        err = ERR_INVALID_INSTRUCTION_TYPE;
    }

    return err;
}

void dumpAssemblyLine(AssemblyLine *line) {
        int i;

        printf("### line ###\n \tcmd: %s\n\tlabel: %s\n\t#args %lu\n", line->opcode_name, line->label, line->arg_count);

        for (i=0; i<line->arg_count; i++) {
            if (i==0) { printf("\t"); }
            printf("arg: %s ", line->args[i]);
        }
        printf("\n");
        printf("\tLineNumber: %d\n", line->debug_info.line_number);
}

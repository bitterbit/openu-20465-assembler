
#include <ctype.h>
#include <stdlib.h>

#include "assembly_line.h"
#include "bit_utils.h"
#include "err.h"
#include "str_utils.h"

ErrorType parseLabel(char **buf, AssemblyLine *line) {
    char *label;

    if (containsChar(*buf, ':') == false) {
        /* if token is not found, Set label to empty string */
        line->label[0] = '\0';
        return SUCCESS;
    }

    /* splitString always returns a string even if delimiter not found */
    label = splitString(buf, ":");
    /* printf("parseLabel label: %s \n", label); */

    if (is_reserved_keyword(label) || strlen(label) > 31 || !isalpha(*label) ||
        containsSpace(label)) {
        /* printf("invalid label: %s\n", label); */
        return ERR_INVALID_LABEL;
    }

    line->flags |= FlagSymbolDeclaration;
    strcpy(line->label, label);

    return SUCCESS;
}

/* TODO: can be replaced with sscanf? */
/* Check if a token is a valid number in the assembly spec */
bool isNumber(char *token) {
    if (token == NULL || *token == '\0')
        return false;
    /* Numbers can start with a sign, but never contain only a sign */
    if (*token == '+' || *token == '-') {
        token++;
        if (!isdigit(*token++)) {
            return false;
        }
    }

    /* All other chars should represent digits */
    while (*token != '\0') {
        if (!isdigit(*token++)) {
            return false;
        }
    }
    return true;
}

/* Return the register number if it is a valid register, -1 otherways */
int registerFromString(char *str) {
    int number;
    int sscanf_success;

    if ((*str++) != '$') {
        return -1;
    }

    sscanf_success = sscanf(str, "%d", &number);

    if (sscanf_success == 0) {
        return -1;
    }

    /* TODO: bug with trailing zeroes */
    if (number < 0 || number > 32) {
        return -1;
    }
    return number;
}

ErrorType parseCommand(char **buf, AssemblyLine *line) {
    ErrorType err = SUCCESS;

    /* split by space or tab */
    char *command = splitString(buf, " \t");
    /* printf("buf=%s buf after splitString\n", *buf); */

    if (*command != '.') {
        if (is_valid_command_name(command)) {
            line->type = TypeCode;
            strcpy(line->opcode_name, command);
        } else {
            err = ERR_INVALID_COMMAND_NAME;
        }
    } else {
        command++;
        if (strArrayIncludes(command, (char **)data_directive_commands,
                             data_directive_commands_len)) {
            line->type = TypeData;
            strcpy(line->opcode_name, command);
        } else if (strArrayIncludes(command, (char **)entry_directive_commands,
                                    entry_directive_commands_len)) {
            line->type = TypeEntry;
            strcpy(line->opcode_name, command);
        } else if (strArrayIncludes(command, (char **)extern_directive_commands,
                                    extern_directive_commands_len)) {
            line->type = TypeExtern;
            strcpy(line->opcode_name, command);
        } else {
            err = ERR_INVALID_COMMAND_NAME;
        }
    }

    return err;
}

char *handleStringToken(char **buf, ErrorType *err) {
    char *token;
    splitString(buf, "\""); /* skip the first quote char */
    token = splitString(
        buf,
        "\""); /* take the value from current buf pointer until next quote */
    return token;
}

/* Returns a new pointer to a token,
    caller is responsible for freeing */
char *handleToken(char **buf, ErrorType *err) {
    char *token;
    char *dynamic_token = NULL;

    /* Remove leading spaces before arg */
    removeLeadingSpaces(buf);

    /* Handle string tokens */
    if (**buf == '"') {
        token = handleStringToken(buf, err);
    }

    /* Handle non string tokens */
    else {
        token = splitString(buf, ",");
    }

    removeTrailingSpaces(&token);

    dynamic_token = malloc(strlen(token) + 1);
    strcpy(dynamic_token, token);

    return dynamic_token;
}

ErrorType parseArgs(char *buf, AssemblyLine *line) {
    char *token = NULL;

    ErrorType err = SUCCESS;
    line->arg_count = 0;
    line->args = NULL;

    while (checkForEmptyLine(buf) != 0) {
        /* token is heap allocated and should be freed when AssemblyLine is
         * freed */
        token = handleToken(&buf, &err);
        line->args =
            realloc(line->args, sizeof(void *) * (line->arg_count + 1));
        line->args[line->arg_count] = token;
        line->arg_count += 1;
    }

    return err;
}

AssemblyLine *newLine() {
    AssemblyLine *line = calloc(1, sizeof(AssemblyLine));
    return line;
}

void freeLine(AssemblyLine *line) {
    /* Clean a line and release the args pointers */
    int i;
    for (i = 0; i < line->arg_count; i++) {
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

    if (err != SUCCESS) {
        return err;
    }

    removeLeadingAndTrailingSpaces((char **)&buf_p);

    /* Handle empty and commented lines */
    if (checkForEmptyLine(buf_p) == 0 || buf_p[0] == COMMENT_CHAR) {
        line->type = TypeEmpty;
        return err;
    }

    err = parseLabel(&buf_p, line);
    /* printf("line after parse label: %s\n", buf_p); */

    if (err != SUCCESS) {
        return err;
    }

    /* If a line doesn't contain a command  - its invalid */
    if (checkForEmptyLine(buf_p) == 0) {
        return ERR_INVALID_COMMAND_FORMAT;
    }

    /* Remove leading spaces before command type */
    removeLeadingSpaces(&buf_p);
    err = parseCommand(&buf_p, line);
    /* printf("line after parse command: %s\n", buf_p); */

    if (err != SUCCESS) {
        return err;
    }

    err = parseArgs(buf_p, line);
    /* printf("line after parse args: %s\n", buf_p); */

    return err;
}

/*
    Returns a buffer with the data that should be written to memory,
    Caller is responsible for freeing the buffer
*/
unsigned char *decodeDataLine(AssemblyLine *line, size_t *out_size,
                              ErrorType *out_err) {
    int i, number, data_chunk_size = -1;

    unsigned char *buf = NULL;
    unsigned char *tmp;

    if (strcmp(ASCIZ, line->opcode_name) == 0) {
        if (line->arg_count != 1) {
            *out_err = ERR_INVALID_DATA_INSTRUCTION;
        }

        /* Copy including the null byte */
        *out_size = strlen(line->args[0]) + 1;
        buf = calloc(*out_size, 1);
        memcpy(buf, line->args[0], *out_size);

    }

    else {

        if (line->arg_count == 0) {
            *out_err = ERR_INVALID_DATA_INSTRUCTION;
        }

        if (strcmp(DB, line->opcode_name) == 0) {
            data_chunk_size = 1;
        } else if (strcmp(DH, line->opcode_name) == 0) {
            data_chunk_size = 2;
        } else if (strcmp(DW, line->opcode_name) == 0) {
            data_chunk_size = 4;
        }

        *out_size = line->arg_count * data_chunk_size;

        /* TODO: i think this isn't needed anymore */
        /* Add 1 more byte for the null byte written by int_to_binary_string */
        buf = calloc(*out_size + 1, 1);
        tmp = buf;

        for (i = 0; i < line->arg_count; i++) {
            *out_err =
                numberFromString(line->args[i], &number, data_chunk_size);

            write_binary_stream_to_buffer(number, data_chunk_size, tmp);
            tmp += data_chunk_size;
        }
    }

    return buf;
}

ErrorType numberFromString(char *str, int *number, int number_of_bits) {
    int sscanf_success;

    sscanf_success = sscanf(str, "%d", number);

    if (sscanf_success != 1) {
        return ERR_INVALID_NUMBER_TOKEN;
    }

    if (!number_fits_in_bits(*number, number_of_bits)) {
        return ERR_INVALID_NUMBER_SIZE;
    }

    return SUCCESS;
}

ErrorType decodeIArithmetic(AssemblyLine *line, Instruction *inst) {
    int temp;
    ErrorType err = SUCCESS;

    /* First arg is a register */
    temp = registerFromString(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->body.i_inst.rs = temp;

    /* Second arg is a number that can be inserted into 16 bits */
    err = numberFromString(line->args[1], &temp, 16);

    if (err != SUCCESS) {
        return err;
    }
    inst->body.i_inst.immed = temp;

    /* Third arg is a register */
    temp = registerFromString(line->args[2]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->body.i_inst.rt = temp;

    return SUCCESS;
}

ErrorType decodeIBranch(AssemblyLine *line, Instruction *inst,
                        SymbolTable *symtab, int instruction_counter) {
    int temp;
    int relative_distance;
    Symbol *sym = NULL;

    /* First arg is a register */
    temp = registerFromString(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->body.i_inst.rs = temp;

    /* Second arg is a register */
    temp = registerFromString(line->args[1]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->body.i_inst.rt = temp;

    /* Third arg is a label */

    sym = symtab->find(symtab, line->args[2]);

    if (sym == NULL) {
        return ERR_UNKNOWN_LABEL_REFERENCED;
    }

    if (sym->is_external == true) {
        return ERR_INVALID_EXTERNAL_LABEL_REFERENCE;
    } else {
        relative_distance = sym->value - instruction_counter;

        /* Make sure relative distance fits immed */
        if (number_fits_in_bits(relative_distance, 16) == false) {
            return ERR_LABEL_TOO_FAR;
        }

        inst->body.i_inst.immed = relative_distance;
    }

    return SUCCESS;
}

/* TODO: check after fix in definitions (page 23) */

/* TODO: Duplicate of IArithmetic */
ErrorType decodeIMem(AssemblyLine *line, Instruction *inst) {
    int temp;
    ErrorType err = SUCCESS;

    /* First arg is a register */
    temp = registerFromString(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->body.i_inst.rs = temp;

    /* Second arg is a number that can be inserted into 16 bits */
    err = numberFromString(line->args[1], &temp, 16);

    if (err != SUCCESS) {
        return err;
    }
    inst->body.i_inst.immed = temp;

    /* Third arg is a register */
    temp = registerFromString(line->args[2]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->body.i_inst.rt = temp;

    return SUCCESS;
}

ErrorType decodeIInstruction(AssemblyLine *line, Instruction *inst,
                             SymbolTable *symtab) {
    ErrorType err = SUCCESS;
    inst->type = I;

    inst->body.r_inst.opcode = command_to_opcode(line->opcode_name, &err);

    if (err != SUCCESS) {
        return err;
    }

    /* Has 3 operands */
    if (line->arg_count != 3) {
        return ERR_INVALID_CODE_INSTRUCTION;
    }

    switch (i_command_to_subtype(line->opcode_name, &err)) {
    case IArithmetic:
        err = decodeIArithmetic(line, inst);
        break;

    case IBranch:
        err = decodeIBranch(line, inst, symtab, line->code_position);
        break;

    case IMem:
        err = decodeIMem(line, inst);
        break;
    }

    return err;
}

ErrorType decodeRArithmetic(AssemblyLine *line, Instruction *inst) {
    int temp;
    /* Has 3 operands */
    if (line->arg_count != 3) {
        return ERR_INVALID_CODE_INSTRUCTION;
    }

    /* First arg is a register */
    temp = registerFromString(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->body.r_inst.rs = temp;

    /* Second arg is a register */
    temp = registerFromString(line->args[1]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->body.r_inst.rt = temp;

    /* Thirds arg is a register */
    temp = registerFromString(line->args[2]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->body.r_inst.rd = temp;

    return SUCCESS;
}

ErrorType decodeRMove(AssemblyLine *line, Instruction *inst) {
    int temp;
    /* Has 2 operands */
    if (line->arg_count != 2) {
        return ERR_INVALID_CODE_INSTRUCTION;
    }

    /* TODO: rd and rs are confused in the explanation?! i switched them now*/

    /* First arg is a register */
    temp = registerFromString(line->args[0]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->body.r_inst.rs = temp;

    /* Second arg is a register */
    temp = registerFromString(line->args[1]);
    if (temp == -1) {
        return ERR_INVALID_REGISTER;
    }
    inst->body.r_inst.rd = temp;

    return SUCCESS;
}

ErrorType decodeRInstruction(AssemblyLine *line, Instruction *inst) {
    ErrorType err = SUCCESS;
    inst->type = R;

    inst->body.r_inst.opcode = command_to_opcode(line->opcode_name, &err);

    if (err != SUCCESS) {
        return err;
    }

    inst->body.r_inst.funct = r_command_to_func(line->opcode_name, &err);

    if (err != SUCCESS) {
        return err;
    }

    switch (r_command_to_subtype(line->opcode_name, &err)) {
    case RArithmetic:
        err = decodeRArithmetic(line, inst);
        break;

    case RMove:
        err = decodeRMove(line, inst);
        break;
    }

    return err;
}

ErrorType decodeJInstruction(AssemblyLine *line, Instruction *inst,
                             SymbolTable *symtab) {
    ErrorType err = SUCCESS;
    int register_number;
    Symbol *sym = NULL;
    inst->type = J;

    inst->body.j_inst.opcode = command_to_opcode(line->opcode_name, &err);

    if (err != SUCCESS) {
        return err;
    }

    /* Use reg = 0 as default as only jump to register changes it to 1 */
    inst->body.j_inst.reg = 0;

    /* STOP */
    if (strcmp(line->opcode_name, STOP) == 0) {
        /* Has 0 operands */
        if (line->arg_count != 0) {
            return ERR_INVALID_CODE_INSTRUCTION;
        }
        return SUCCESS;
    }

    else {
        /* Has 1 operand */
        if (line->arg_count != 1) {
            return ERR_INVALID_CODE_INSTRUCTION;
        }

        register_number = registerFromString(line->args[0]);
        /* if JMP and first arg is a register */
        if (strcmp(line->opcode_name, JMP) == 0 && register_number != -1) {
            inst->body.j_inst.address = register_number;
            inst->body.j_inst.reg = 1;
        }

        /* JMP with label, LA, CALL */
        else {
            sym = symtab->find(symtab, line->args[0]);

            if (sym == NULL) {
                return ERR_UNKNOWN_LABEL_REFERENCED;
            }

            if (sym->is_external == true) {
                inst->body.j_inst.address = 0;
            } else {
                inst->body.j_inst.address = sym->value;
            }
        }
    }

    return SUCCESS;
}

ErrorType decodeInstructionLine(AssemblyLine *line, Instruction *inst,
                                SymbolTable *symtab) {
    ErrorType err = SUCCESS;

    /* is i instruction */
    if (is_i_command(line->opcode_name)) {
        err = decodeIInstruction(line, inst, symtab);
        /*         int immed = inst->body.i_inst.immed;
                int rt = inst->body.i_inst.rt;
                int rs = inst->body.i_inst.rs;
                int opcode = inst->body.i_inst.opcode;

                printf("%d, %d, %d, %d\n", immed, rt, rs, opcode); */

    }

    /* is r instruction */
    else if (is_r_command(line->opcode_name)) {
        err = decodeRInstruction(line, inst);
        /*         int unused = inst->body.r_inst.unused;
                int funct = inst->body.r_inst.funct;
                int rd = inst->body.r_inst.rd;
                int rt = inst->body.r_inst.rt;
                int rs = inst->body.r_inst.rs;
                int opcode = inst->body.r_inst.opcode;

                printf("%d, %d, %d, %d, %d, %d\n", unused, funct, rd, rt, rs,
           opcode); */
    }

    /* is j instruction */
    else if (is_j_command(line->opcode_name)) {
        err = decodeJInstruction(line, inst, symtab);
        /*         int address = inst->body.j_inst.address;
                int reg = inst->body.j_inst.reg;
                int opcode = inst->body.j_inst.opcode;

                printf("%d, %d, %d\n", address, reg, opcode); */
    }

    else {
        err = ERR_INVALID_INSTRUCTION_TYPE;
    }

    return err;
}

void dumpAssemblyLine(AssemblyLine *line) {
    int i;

    printf("### line ###\n \tcmd: %s\n\tlabel: %s\n\t#args %lu\n",
           line->opcode_name, line->label, line->arg_count);

    for (i = 0; i < line->arg_count; i++) {
        if (i == 0) {
            printf("\t");
        }
        printf("arg: %s ", line->args[i]);
    }
    printf("\n");
    printf("\tLineNumber: %d\n", line->debug_info.line_number);
}

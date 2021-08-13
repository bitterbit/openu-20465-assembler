
#include "instruction.h"
#include "str_utils.h"

/* funct opcode, and subtype mappings are based on the order of the commands
 * arrays */
const char *r_commands[] = {ADD, SUB, AND, OR, NOR, MOVE, MVHI, MVLO};
const int r_commands_len = 8;
const int r_commands_funct[] = {1, 2, 3, 4, 5, 1, 2, 3};
const int r_commands_opcode[] = {0, 0, 0, 0, 0, 1, 1, 1};
const RInstructionSubType r_commands_subtype[] = {
    RArithmetic, RArithmetic, RArithmetic, RArithmetic,
    RArithmetic, RMove,       RMove,       RMove};

const char *i_commands[] = {ADDI, SUBI, ANDI, ORI, NORI, BNE, BEQ, BLT,
                            BGT,  LB,   SB,   LW,  SW,   LH,  SH};
const int i_commands_len = 15;
const int i_commands_opcode[] = {10, 11, 12, 13, 14, 15, 16, 17,
                                 18, 19, 20, 21, 22, 23, 24};
const IInstructionSubType i_commands_subtype[] = {
    IArithmetic, IArithmetic, IArithmetic, IArithmetic, IArithmetic,
    IBranch,     IBranch,     IBranch,     IBranch,     IMem,
    IMem,        IMem,        IMem,        IMem,        IMem};

const char *j_commands[] = {JMP, LA, CALL, STOP};
const int j_commands_len = 4;
const int j_commands_opcode[] = {30, 31, 32, 63};

const char *data_directive_commands[] = {DB, DH, DW, ASCIZ};
const int data_directive_commands_len = 4;

const char *entry_directive_commands[] = {ENTRY};
const int entry_directive_commands_len = 1;

const char *extern_directive_commands[] = {EXTERN};
const int extern_directive_commands_len = 1;

bool is_valid_command_name(char *command) {
    return is_i_command(command) | is_r_command(command) |
           is_j_command(command);
}

bool is_reserved_keyword(char *str) {

    bool is_reserved = is_valid_command_name(str);

    /* is directive */
    is_reserved |= strArrayIncludes(str, (char **)data_directive_commands,
                                    data_directive_commands_len);
    is_reserved |= strArrayIncludes(str, (char **)entry_directive_commands,
                                    entry_directive_commands_len);
    is_reserved |= strArrayIncludes(str, (char **)extern_directive_commands,
                                    extern_directive_commands_len);

    return is_reserved;
}

bool is_i_command(char *command) {
    return strArrayIncludes(command, (char **)i_commands, i_commands_len);
}

bool is_r_command(char *command) {
    return strArrayIncludes(command, (char **)r_commands, r_commands_len);
}

bool is_j_command(char *command) {
    return strArrayIncludes(command, (char **)j_commands, j_commands_len);
}

int command_to_mapped_value(char *command, char **commands_array,
                            int command_array_length, int *mapping,
                            ErrorType *out_err) {
    int command_index =
        findInArray(command, (char **)commands_array, command_array_length);

    if (command_index >= 0) {
        return mapping[command_index];
    }
    *out_err = ERR_INVALID_COMMAND_NAME;
    return 0;
}

int command_to_opcode(char *command, ErrorType *out_err) {
    if (is_i_command(command)) {
        return command_to_mapped_value(command, (char **)i_commands,
                                       i_commands_len, (int *)i_commands_opcode,
                                       out_err);
    }

    if (is_r_command(command)) {
        return command_to_mapped_value(command, (char **)r_commands,
                                       r_commands_len, (int *)r_commands_opcode,
                                       out_err);
    }

    if (is_j_command(command)) {
        return command_to_mapped_value(command, (char **)j_commands,
                                       j_commands_len, (int *)j_commands_opcode,
                                       out_err);
    }

    *out_err = ERR_INVALID_COMMAND_NAME;
    return 0;
}

int r_command_to_func(char *command, ErrorType *out_err) {
    return command_to_mapped_value(command, (char **)r_commands, r_commands_len,
                                   (int *)r_commands_funct, out_err);
}

RInstructionSubType r_command_to_subtype(char *command, ErrorType *out_err) {
    return command_to_mapped_value(command, (char **)r_commands, r_commands_len,
                                   (int *)r_commands_subtype, out_err);
}

IInstructionSubType i_command_to_subtype(char *command, ErrorType *out_err) {
    return command_to_mapped_value(command, (char **)i_commands, i_commands_len,
                                   (int *)i_commands_subtype, out_err);
}

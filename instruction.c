
#include "instruction.h"
#include "str_utils.h"


/* TODO: probably not the nicest way to do this */
const char* r_commands[] = {"add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo"};
const int r_commands_len = 8;
const char* i_commands[] = {"addi", "subi", "andi", "ori", "nori", "beq", "bne", "blt", "bgt", "lb", "sb", "lw", "sw", "lh", "sh"};
const int i_commands_len = 15;
const char* j_commands[] = {"jmp", "la", "call", "stop"};
const int j_commands_len = 4;
const char* data_directive_commands[] = {"dd", "dw", "db", "asciz"};
const int data_directive_commands_len = 4;
const char* entry_directive_commands[] = {"entry"};
const int entry_directive_commands_len = 1;
const char* extern_directive_commands[] = {"extern"};
const int extern_directive_commands_len = 1;


/* TODO: make efficient? */
bool is_code_opcode(char* str){
    /* is i instruction */
    bool is_reserved = str_in_str_array(str, (char**)i_commands, i_commands_len);

    /* is r instruction */
    is_reserved |= str_in_str_array(str, (char**)r_commands, r_commands_len);
    
    /* is j instruction */
    is_reserved |= str_in_str_array(str, (char**)j_commands, j_commands_len);

    return is_reserved;
}


/* TODO: make efficient? */
bool is_reserved_keyword(char* str){

    bool is_reserved = is_code_opcode(str);
    
    /* is directive */
    is_reserved |= str_in_str_array(str, (char**)data_directive_commands, data_directive_commands_len);
    is_reserved |= str_in_str_array(str, (char**)entry_directive_commands, entry_directive_commands_len);
    is_reserved |= str_in_str_array(str, (char**)extern_directive_commands, extern_directive_commands_len);

    return is_reserved;
}

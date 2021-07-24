
#include "instruction.h"
#include "str_utils.h"


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

    bool is_reserved  = is_code_opcode(str);
    
    /* is directive */
    is_reserved |= str_in_str_array(str, (char**)data_directive_commands, data_directive_commands_len);
    is_reserved |= str_in_str_array(str, (char**)entry_directive_commands, entry_directive_commands_len);

    return is_reserved;
}

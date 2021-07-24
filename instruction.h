#ifndef _H_INSTRUCTION_
#define _H_INSTRUCTION_

#include "bool.h"

typedef enum {
    R,
    I,
    J
} InstructionType;

typedef struct RInstruction RInstruction;
typedef struct IInstruction IInstruction;
typedef struct JInstruction JInstruction;
typedef struct Instruction Instruction;

/* R Instructions: add, sub, and, or, nor, move, mvhi, mvlo */
struct RInstruction {
    unsigned int unused: 6;
    unsigned int funct: 5;
    unsigned int rd: 5;
    unsigned int rt: 5;
    unsigned int rs: 5;
    unsigned int opcode: 6;
};

const char* r_commands[] = {"add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo"};
const int r_commands_len = 8;

/* I Instructions: 
 *   regular: addi, subi, andi, ori, nori, 
 *   labels: beq, bne, blt, bgt, lb, sb, lw, sw, lh, sh */
struct IInstruction {
    unsigned int immed: 16;
    unsigned int rt: 5;
    unsigned int rs: 5;
    unsigned int opcode: 6;
};

const char* i_commands[] = {"addi", "subi", "andi", "ori", "nori", "beq", "bne", "blt", "bgt", "lb", "sb", "lw", "sw", "lh", "sh"};
const int i_commands_len = 15;

/* J Instructions: jmp, la, call, stop */
struct JInstruction {
    unsigned int address: 25;
    unsigned int reg: 1;
    unsigned int opcode: 6;
};

const char* j_commands[] = {"jmp", "la", "call", "stop"};
const int j_commands_len = 4;

struct Instruction {
    InstructionType type;
    union {
        RInstruction r_inst;
        IInstruction i_inst;
        JInstruction j_inst;
    } instruction;
};


/* TODO: Move */
const char* data_directive_commands[] = {"dd", "dw", "db", "asciz"};
const int data_directive_commands_len = 4;

const char* entry_directive_commands[] = {"entry", "extern"};
const int entry_directive_commands_len = 2;

bool is_reserved_keyword(char* str);
bool is_code_opcode(char* str);

#endif

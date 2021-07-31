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

extern const char* r_commands[];
extern const int r_commands_len;

/* I Instructions: 
 *   regular: addi, subi, andi, ori, nori, 
 *   labels: beq, bne, blt, bgt, lb, sb, lw, sw, lh, sh */
struct IInstruction {
    unsigned int immed: 16;
    unsigned int rt: 5;
    unsigned int rs: 5;
    unsigned int opcode: 6;
};

extern const char* i_commands[];
extern const int i_commands_len;

/* J Instructions: jmp, la, call, stop */
struct JInstruction {
    unsigned int address: 25;
    unsigned int reg: 1;
    unsigned int opcode: 6;
};

extern const char* j_commands[];
extern const int j_commands_len;

struct Instruction {
    InstructionType type;
    union {
        RInstruction r_inst;
        IInstruction i_inst;
        JInstruction j_inst;
    } instruction;
};


/* TODO: Move */
extern const char* data_directive_commands[];
extern const int data_directive_commands_len;

extern const char* entry_directive_commands[];
extern const int entry_directive_commands_len;
extern const char* extern_directive_commands[];
extern const int extern_directive_commands_len;

bool is_reserved_keyword(char* str);
bool is_code_opcode(char* str);

#endif

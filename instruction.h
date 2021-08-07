#ifndef _H_INSTRUCTION_
#define _H_INSTRUCTION_

#include "bool.h"

#define INSTRUCTION_SIZE 4

/* Define all constant command and directive names */
#define ADD "add"
#define SUB "sub"
#define AND "and"
#define OR "or"
#define NOR "nor"
#define MOVE "move"
#define MVHI "mvhi"
#define MVLO "mvlo"

#define ADDI "addi"
#define SUBI "subi"
#define ANDI "andi"
#define ORI "ori"
#define NORI "nori"
#define BEQ "beq"
#define BNE "bne"
#define BLT "blt"
#define BGT "bgt"
#define LB "lb"
#define SB "sb"
#define LW "lw"
#define SW "sw"
#define LH "lh"
#define SH "sh"

#define JMP "jmp"
#define LA "la"
#define CALL "call"
#define STOP "stop"

#define DB "db"
#define DH "dh"
#define DW "dw"
#define ASCIZ "asciz"

#define ENTRY "entry"

#define EXTERN "extern"


typedef enum {
    R,
    I,
    J
} InstructionType;


typedef enum {
    RArithmetic,
    RMove
} RInstructionSubType;

typedef enum {
    IArithmetic,
    IBranch,
    IMem
} IInstructionSubType;


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
        unsigned int inst; /* typeless value of this instruction */
    } body;
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
bool is_i_command(char *command);
bool is_r_command(char *command);
bool is_j_command(char *command);
int command_to_opcode(char *command);
int r_command_to_func(char *command);
RInstructionSubType r_command_to_subtype(char *command);
IInstructionSubType i_command_to_subtype(char *command);

#endif

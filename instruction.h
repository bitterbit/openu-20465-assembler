
typedef enum {
    R,
    I,
    J,
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
    unsigned opcode: 6;
};

/* I Instructions: addi, subi, andi, ori, nori, beg, bne, blt, bgt, lb, sb, lw, sw, lh, sh */
struct IInstruction {
    unsigned int immed: 16;
    unsigned int rt: 5;
    unsigned int rs: 5;
    unsigned int opcode: 6;
};

/* J Instructions: jmp, la, call, stop */
struct JInstruction {
    unsigned int address: 25;
    unsigned int reg: 1;
    unsigned int opcode: 6;
};

struct Instruction {
    InstructionType type;
    union {
        RInstruction r_inst;
        IInstruction i_inst;
        JInstruction j_inst;
    };
};

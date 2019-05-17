#ifndef _INCLUDE_INSTRUCTION_H
#define _INCLUDE_INSTRUCTION_H

#include <stdint.h>

/// opcode
// R-type
#define OP_REG_ARITH 0x33
// I-type
#define OP_LOAD 0x03
#define OP_IMM_ARITH 0x13
#define OP_REG_JUMP 0x67
// S-type
#define OP_STORE 0x23



typedef struct instr {
    union {
        // opcode
        unsigned opcode : 7;

        // type R
        struct {
            unsigned : 7;
            unsigned rd : 5;
            unsigned func3 : 3;
            unsigned rs1 : 5;
            unsigned rs2 : 5;
            unsigned func7 : 7;
        } r_type;

        // type I
        struct {
            unsigned : 7;
            unsigned rd : 5;
            unsigned func3 : 3;
            unsigned rs1 : 5;
            int imm : 12;
        } i_type;

        // type S
        struct {
            unsigned : 7;
            unsigned imm1 : 5;
            unsigned func3 : 3;
            unsigned rs1 : 5;
            unsigned rs2 : 5;
            int imm2 : 7;
        } s_type;
    };
} Instr;

#endif /* _INCLUDE_INSTRUCTION_H_ */

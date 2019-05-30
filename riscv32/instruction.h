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

// B-type
#define OP_COND_BRANCH 0x63

// J- type
#define OP_UNCOND_JUMP 0x6f

// U-type
#define OP_LOAD_UPPER_IMM 0x37
#define OP_ADD_UPPER_IMM_PC 0x17

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

        // type B
        struct {
            unsigned : 7;
            unsigned i1 : 1;
            unsigned i2 : 4;
            unsigned func3 : 3;
            unsigned rs1 : 5;
            unsigned rs2 : 5;
            unsigned i3 : 6;
            int i4 : 1;
        } b_type;

        // type J
        struct {
            unsigned : 7;
            unsigned rd : 5;
            int imm : 20;
        } j_type;

        // type U
        struct {
            unsigned : 7;
            unsigned rd : 5;
            unsigned imm : 20;
        } u_type;
    };
} Instr;

static int decode_instr(int code, Instr *instr)
{
    int opcode = code & 0x7f;
    switch (opcode) {
    // r type
    case OP_REG_ARITH:
        instr->r_type.rd = code >> 7 & 0x1f;
        instr->r_type.func3 = code >> 12 & 0x7;
        instr->r_type.rs1 = code >> 15 & 0x1f;
        instr->r_type.rs2 = code >> 20 & 0x1f;
        instr->r_type.func7 = code >> 25 & 0x7f;
        break;

    // i type
    case OP_LOAD:
    case OP_IMM_ARITH:
    case OP_REG_JUMP:
        instr->i_type.rd = code >> 7 & 0x1f;
        instr->i_type.func3 = code >> 12 & 0x7;
        instr->i_type.rs1 = code >> 15 & 0x1f;
        instr->i_type.imm = code >> 20;
        break;

    // s type
    case OP_STORE:
        instr->s_type.imm1 = code >> 7 & 0x1f;
        instr->s_type.func3 = code >> 12 & 0x7;
        instr->s_type.rs1 = code >> 15 & 0x1f;
        instr->s_type.rs2 = code >> 20 & 0x1f;
        instr->s_type.imm2 = code >> 25;
        break;

    // B type
    case OP_COND_BRANCH:
        instr->b_type.i1 = code >> 7 & 0x1;
        instr->b_type.i2 = code >> 8 & 0xf;
        instr->b_type.func3 = code >> 12 & 0x7;
        instr->b_type.rs1 = code >> 15 & 0x1f;
        instr->b_type.rs2 = code >> 20 & 0x1f;
        instr->b_type.i3 = code >> 25 & 0x3f;
        instr->b_type.i4 = code >> 31;
        break;

    // J type
    case OP_UNCOND_JUMP:
        instr->j_type.rd = code >> 7 & 0x1f;
        instr->j_type.imm = (code & 1 << 31) >> 11 |
                            (code & 0x3ff << 21) >> 20 | (code & 1 << 20) >> 9 |
                            (code & 0xff << 12);
        break;

    // U type
    case OP_LOAD_UPPER_IMM:
    case OP_ADD_UPPER_IMM_PC:
        instr->u_type.rd = code >> 7 & 0x1f;
        instr->u_type.imm = code >> 12;
        break;

    default:
        printf("%d\n", opcode);
        opcode = -1;
    }

    return opcode;
}
#endif /* _INCLUDE_INSTRUCTION_H_ */

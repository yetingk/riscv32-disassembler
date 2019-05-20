#include <stdbool.h>
#include <stdio.h>
#include "riscv32/instruction.h"
#include "riscv32/register.h"

int decode_binary(void *code)
{
    const char *op = NULL;
    int16_t imm = 0;
    Instr *instr = (Instr *) code;

    // register arithmetic
    if (instr->opcode == OP_REG_ARITH) {
        switch (instr->r_type.func3) {
        case 0:
            if (instr->r_type.func7 == 0x00) {
                op = "add ";
            } else {  // instr->r_type.func7 == 0x20
                op = "sub ";
            }
            break;
        case 1:
            op = "sll ";
            break;
        case 2:
            op = "slt ";
            break;
        case 3:
            op = "sltu";
            break;
        case 4:
            op = "xor ";
            break;
        default:
            goto err;
        }

        return printf("%s %s,%s,%s\n", op, reg_name[instr->r_type.rd],
                      reg_name[instr->r_type.rs1], reg_name[instr->r_type.rs2]);
    }

    // immediate arithmetic
    if (instr->opcode == OP_IMM_ARITH) {
        switch (instr->i_type.func3) {
        case 0:
            // mv rd,rs1 is mean "addi rd,rs1,0"
            if (instr->i_type.imm == 0)
                return printf("mv   %s,%s\n", reg_name[instr->r_type.rd],
                              reg_name[instr->r_type.rs1]);

            op = "addi";
            break;
        default:
            goto err;
        }

        return printf("%s %s,%s,%d\n", op, reg_name[instr->i_type.rd],
                      reg_name[instr->i_type.rs1], instr->i_type.imm);
    }

    // load instruction
    if (instr->opcode == OP_LOAD) {
        switch (instr->i_type.func3) {
        case 2:
            op = "lw  ";
            break;
        default:
            goto err;
        }
        return printf("%s %s,%d(%s)\n", op, reg_name[instr->i_type.rd],
                      instr->i_type.imm, reg_name[instr->i_type.rs1]);
    }

    // store instruction
    if (instr->opcode == OP_STORE) {
        imm = instr->s_type.imm2 << 5 | instr->s_type.imm1;
        switch (instr->i_type.func3) {
        case 2:
            op = "sw  ";
            break;
        default:
            goto err;
        }
        return printf("%s %s,%d(%s)\n", op, reg_name[instr->i_type.rd], imm,
                      reg_name[instr->i_type.rs1]);
    }

    // register jump
    if (instr->opcode == OP_REG_JUMP) {
        if (instr->i_type.rd != REG_ZERO || instr->i_type.imm != 0)
            return printf("jalr %s %d(%s)\n", reg_name[instr->i_type.rd],
                          instr->i_type.imm, reg_name[instr->i_type.rs1]);

        // jr rs1 ::= jalr,zero,0(rs1)
        if (instr->i_type.rs1 != REG_RA)
            return printf("jr %s\n", reg_name[instr->i_type.rs1]);

        // ret ::= jr ra
        return printf("ret\n");
    }

err:
    printf("%8x\n", *(int *) code);
    return -1;
}

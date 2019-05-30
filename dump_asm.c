#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "riscv32/instruction.h"
#include "riscv32/register.h"

int dump_asm(int opcode, Instr *instr, uint32_t addr)
{
    const char *op = NULL;
    int imm = 0;

    switch (opcode) {
    // register arithmetic
    case OP_REG_ARITH:
        switch (instr->r_type.func3) {
        case 0:
            if (instr->r_type.func7 == 0x00) {
                op = "add  ";
            } else {  // instr.r_type.func7 == 0x20
                op = "sub  ";
            }
            break;
        case 1:
            op = "sll  ";
            break;
        case 2:
            op = "slt  ";
            break;
        case 3:
            op = "sltu ";
            break;
        case 4:
            op = "xor  ";
            break;
        case 5:
            if (instr->r_type.func7 == 0x00) {
                op = "srl  ";
            } else {  // instr.r_type.func7 == 0x20
                op = "sra  ";
            }
            break;
        case 6:
            op = "or   ";
            break;
        case 7:
            op = "and  ";
            break;

        default:
            goto err;
        }

        return printf("%08x %s %s,%s,%s\n", addr, op,
                      reg_name[instr->r_type.rd], reg_name[instr->r_type.rs1],
                      reg_name[instr->r_type.rs2]);

    // immediate arithmetic
    case OP_IMM_ARITH:
        switch (instr->i_type.func3) {
        case 0:
            
			// li rd imm ::= addi rd zero imm
			if (instr->i_type.rs1 == REG_ZERO && instr->i_type.rd != REG_ZERO)
                return printf("%08x li    %s,%d\n", addr,
                              reg_name[instr->i_type.rd], instr->i_type.imm);
            
			if (instr->i_type.imm == 0) {
                // nop ::= addi zero,zero,0
                if (instr->i_type.rd == REG_ZERO &&
                    instr->i_type.rs1 == REG_ZERO)
                    return printf("%08x nop\n", addr);

                // mv rd,rs1 ::= addi rd,rs1,0
                return printf("%08x mv    %s,%s\n", addr,
                              reg_name[instr->r_type.rd],
                              reg_name[instr->r_type.rs1]);
            }
            op = "addi ";
            break;
        case 1:
            return printf(
                "%08x slli  %s,%s,%d\n", addr, reg_name[instr->i_type.rd],
                reg_name[instr->i_type.rs1], instr->i_type.imm & 0x1f);
        case 2:
            op = "slti ";
            break;
        case 3:
            op = "sltiu";
            break;
        case 4:
            op = "xori ";
            break;
        case 5:
            if ((instr->i_type.imm >> 6 & 0x1f) == 0) {
                op = "srli ";
            } else {  // func7 == 0x10
                op = "srai ";
            }
            return printf(
                "%08x %s %s,%s,%d\n", addr, op, reg_name[instr->i_type.rd],
                reg_name[instr->i_type.rs1], instr->i_type.imm & 0x1f);
        case 6:
            op = "ori  ";
            break;
        case 7:
            op = "andi ";
            break;
        default:
            goto err;
        }

        return printf("%08x %s %s,%s,%d\n", addr, op,
                      reg_name[instr->i_type.rd], reg_name[instr->i_type.rs1],
                      instr->i_type.imm);

    // load instruction
    case OP_LOAD:
        switch (instr->i_type.func3) {
        case 0:
            op = "lb   ";
            break;
        case 1:
            op = "lh   ";
            break;
        case 2:
            op = "lw   ";
            break;
        case 4:
            op = "ulb  ";
            break;
        case 5:
            op = "ulh  ";
            break;
        case 6:
            op = "ulw  ";
            break;
        default:
            goto err;
        }
        return printf("%08x %s %s,%d(%s)\n", addr, op,
                      reg_name[instr->i_type.rd], instr->i_type.imm,
                      reg_name[instr->i_type.rs1]);

    // store instruction
    case OP_STORE:
        imm = instr->s_type.imm2 << 5 | instr->s_type.imm1;
        switch (instr->s_type.func3) {
        case 0:
            op = "sb   ";
            break;
        case 1:
            op = "sh   ";
            break;
        case 2:
            op = "sw   ";
            break;
        default:
            goto err;
        }
        return printf("%08x %s %s,%d(%s)\n", addr, op,
                      reg_name[instr->s_type.rs2], imm,
                      reg_name[instr->s_type.rs1]);

    // conditional branch
    case OP_COND_BRANCH:
        imm = instr->b_type.i4 << 12 | instr->b_type.i2 << 1 |
              instr->b_type.i3 << 5 | instr->b_type.i1 << 11;
        imm += addr;

        switch (instr->b_type.func3) {
        case 0:
            op = "beq  ";
            break;
        case 1:
            op = "bne  ";
            break;
        case 4:
            op = "blt  ";
            break;
        case 5:
            op = "bge  ";
            break;
        case 6:
            op = "bltu ";
            break;
        case 7:
            op = "bgeu ";
            break;
        }
        return printf("%08x %s %s %s %d\n", addr, op,
                      reg_name[instr->b_type.rs1], reg_name[instr->b_type.rs2],
                      imm);

    case OP_UNCOND_JUMP:
        imm = instr->j_type.imm + addr;
        if (instr->j_type.rd == REG_ZERO)
            return printf("%08x j     %d\n", addr, imm);
        return printf("%08x jal   %s,%d\n", addr, reg_name[instr->j_type.rd],
                      instr->j_type.imm);


    // register jump
    case OP_REG_JUMP:
        if (instr->i_type.rd != REG_ZERO || instr->i_type.imm != 0)
            return printf("%08x jalr  %s %d(%s)\n", addr,
                          reg_name[instr->i_type.rd], instr->i_type.imm,
                          reg_name[instr->i_type.rs1]);

        // jr rs1 ::= jalr,zero,0(rs1)
        if (instr->i_type.rs1 != REG_RA)
            return printf("%08x, jr    %s\n", addr,
                          reg_name[instr->i_type.rs1]);

        // ret ::= jr ra
        return printf("%08x ret\n", addr);

    // load upper immediate
    case OP_LOAD_UPPER_IMM:
        return printf("%08x lui   %s %d\n", addr, reg_name[instr->u_type.rd],
                      instr->u_type.imm);

    // auipc
    case OP_ADD_UPPER_IMM_PC:
        return printf("%08x auipc %s %d\n", addr, reg_name[instr->u_type.rd],
                      instr->u_type.imm);
    }

err:
    return -1;
}

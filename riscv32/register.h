#ifndef _INCLUDE_REGISTER_H_
#define _INCLUDE_REGISTER_H_

#include <string.h>

// register file
enum {
    REG_ZERO = 0x0,  // x0 / zero
    REG_RA,          // x1 / ra
    REG_SP,          // x2 / sp
    REG_GP,          // x3 / gp
    REG_TP,          // x4 / tp
    REG_T0,          // x5 / t0
    REG_T1,          // x6 / t1
    REG_T2,          // x7 / t2
    REG_FP,          // x8 / s0 / fp
    REG_S1,          // x9 / s1
    REG_A0,          // x10 / a0
    REG_A1,          // x11 / a1
    REG_A2,          // x12 / a2
    REG_A3,          // x13 / a3
    REG_A4,          // x14 / a4
    REG_A5,          // x15 / a5
    REG_A6,          // x16 / a6
    REG_A7,          // x17 / a7
    REG_S2,          // x18 / s2
    REG_S3,          // x19 / s3
    REG_S4,          // x20 / s4
    REG_S5,          // x21 / s5
    REG_S6,          // x22 / s6
    REG_S7,          // x23 / s7
    REG_S8,          // x24 / s8
    REG_S9,          // x25 / s9
    REG_S10,         // x26 / s10
    REG_S11,         // x27 / s11
    REG_T3,          // x28 / t3
    REG_T4,          // x29 / t4
    REG_T5,          // x30 / t5
    REG_T6           // x31 / t6
};

#define REG_NUM 32

static const char *reg_name[REG_NUM] = {
    "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
    "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
    "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

static inline int reg_name_to_num(const char *word)
{
    for (int i = 0; i < REG_NUM; ++i) {
        if (!strcmp(word, reg_name[i]))
            return i;
    }

    return -1;
}

#endif  //_INCLUDE_REGISTER_H_

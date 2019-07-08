#pragma once

/*
** The enum can be found in <ucontext.h> but is
** only defined upon _GNU_SOURCE definition
*/

typedef enum
{
 REG_FIRST = 0,
 REG_R8 = 0,
 REG_R9,
 REG_R10,
 REG_R11,
 REG_R12,
 REG_R13,
 REG_R14,
 REG_R15,
 REG_RDI,
 REG_RSI,
 REG_RBP,
 REG_RBX,
 REG_RDX,
 REG_RAX,
 REG_RCX,
 REG_RSP,
 REG_RIP,
 REG_EFL,
 REG_CSGSFS,
 REG_ERR,
 REG_TRAPNO,
 REG_OLDMASK,
 REG_CR2,
 REG_LAST
} e_register;

extern const char* const reg_names[];

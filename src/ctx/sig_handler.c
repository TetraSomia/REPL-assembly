#include <stdio.h> //TODO tmp
#include "sig_handler.h"
#include "reg_idx.h"
#include "repl.h"
#include "context_switch.h"

#define TRAP_FLAG_MASK 0x0100

static inline void _reset_breakpoint(ucontext_t *ucontext,
				     s_code_instruction *inst) {
  ucontext->uc_mcontext.gregs[REG_EFL] ^= 0x0100;
  if (inst->breakpoint)
    set_breakpoint(inst);
}

static inline void _set_trap_flag(ucontext_t *ucontext) {
  ucontext->uc_mcontext.gregs[REG_EFL] |= TRAP_FLAG_MASK;
}

void breakpoint_handler(int sig, siginfo_t *siginfo, void *raw_context) {
  ucontext_t *ucontext = (ucontext_t*)raw_context;
  static s_code_instruction *inst = NULL;
  (void)sig;
  (void)siginfo;

  if (ucontext->uc_mcontext.gregs[REG_EFL] & TRAP_FLAG_MASK) {
    _reset_breakpoint(ucontext, inst);
    return;
  }
  context.exec_ctx = ucontext;
  ucontext->uc_mcontext.gregs[REG_RIP] -= 1; //reset RIP to inst first byte
  printf("Breakpoint hit: %p\n", (void*)ucontext->uc_mcontext.gregs[REG_RIP]);
  ctx_resume_repl();
  inst = inst_find_from_addr((void*)ucontext->uc_mcontext.gregs[REG_RIP]);
  if (!inst)
    fatal_err("Instruction not found from RIP\n");
  *(inst->address) = inst->opcodes[0]; //replace breakpoint by inst first byte
  _set_trap_flag(ucontext);
}

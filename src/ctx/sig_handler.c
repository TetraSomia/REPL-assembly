#include <stdio.h>
#include <signal.h>
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
  set_exec_sighandlers();
}

void crash_handler(int sig) {
  const struct {int signum; const char *name; const char *desc;}
  strs[] = {{SIGFPE, "SIGFPE", sys_siglist[SIGFPE]},
	    {SIGILL, "SIGILL", sys_siglist[SIGILL]},
	    {SIGSEGV, "SIGSEGV", sys_siglist[SIGSEGV]}};

  for (size_t i = 0; i < sizeof(strs) / sizeof(*strs); ++i)
    if (sig == strs[i].signum) {
      fprintf(stderr, "Crash of execution due to %s: %s\n",
	      strs[i].name, strs[i].desc);
      break;
    }
  ctx_abort_exec();
}

void reset_exec_sighandlers() {
  signal(SIGFPE, SIG_DFL);
  signal(SIGILL, SIG_DFL);
  signal(SIGSEGV, SIG_DFL);
  signal(SIGTRAP, SIG_DFL);
}

void set_exec_sighandlers() {
  struct sigaction sigact_struct = {0};
  int ret = 0;

  sigact_struct.sa_handler = &crash_handler;
  sigact_struct.sa_flags = 0;
  sigemptyset(&sigact_struct.sa_mask);
  ret |= sigaction(SIGFPE, &sigact_struct, NULL);
  ret |= sigaction(SIGILL, &sigact_struct, NULL);
  ret |= sigaction(SIGSEGV, &sigact_struct, NULL);
  sigact_struct.sa_sigaction = &breakpoint_handler;
  sigact_struct.sa_flags |= SA_SIGINFO;
  ret |= sigaction(SIGTRAP, &sigact_struct, NULL);
  if (ret != 0)
    fatal_libc_err("sigaction failed\n");
}

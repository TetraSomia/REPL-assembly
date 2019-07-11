#include <stdio.h>
#include <signal.h>
#include "reg_idx.h"
#include "repl.h"
#include "context_switch.h"
#include "getters.h"

#define TRAP_FLAG_MASK 0x0100

static inline void _reset_breakpoint(s_code_instruction *inst) {
  *get_reg_ptr(REG_EFL) ^= TRAP_FLAG_MASK;
  if (inst->breakpoint)
    set_breakpoint(inst);
}

static void _set_cur_unit() {
  s_code_unit *cur_unit = unit_find_from_addr((void*)get_reg(REG_RIP));

  if (cur_unit != NULL)
    context.cur_unit = cur_unit;
}

static void _breakpoint_handler(int sig, siginfo_t *info, void *raw_context) {
  ucontext_t *ucontext = (ucontext_t*)raw_context;
  static s_code_instruction *inst = NULL;

  (void)info;
  context.exec_ctx = ucontext;
  if (get_reg(REG_EFL) & TRAP_FLAG_MASK) {
    _reset_breakpoint(inst);
    return;
  }
  _set_cur_unit();
  if (sig == SIGTRAP) {
    *get_reg_ptr(REG_RIP) -= 1; //reset RIP to inst first byte
    printf("Breakpoint hit (%p)\n", (void*)get_reg(REG_RIP));
  }
  else
    fprintf(stderr, "Execution stopped manually\n");
  ctx_resume_repl();
  if (sig == SIGTRAP) {
    inst = inst_find_from_addr((void*)get_reg(REG_RIP));
    if (!inst)
      fatal_err("Instruction not found from RIP\n");
    *(inst->address) = inst->opcodes[0]; //replace breakpoint by inst first byte
    *get_reg_ptr(REG_EFL) |= TRAP_FLAG_MASK;
  }
  set_exec_sighandlers();
}

static void _crash_handler(int sig, siginfo_t *info, void *raw_context) {
  ucontext_t *ucontext = (ucontext_t*)raw_context;
  typedef struct {int no; const char *desc;} s_sicode;
  const s_sicode code_ill[] = {{ILL_ILLOPC, "Illegal opcode"},
			       {ILL_ILLOPN, "Illegal operand"},
			       {ILL_ILLADR, "Illegal addressing mode"},
			       {ILL_ILLTRP, "Illegal trap"},
			       {ILL_PRVOPC, "Privileged opcode"},
			       {ILL_PRVREG, "Privileged register"},
			       {ILL_COPROC, "Coprocessor error"},
			       {ILL_BADSTK, "Internal stack error"},
			       {0, NULL}};
  const s_sicode code_fpe[] = {{FPE_INTDIV, "Integer divide-by-zero"},
			       {FPE_INTOVF, "Integer overflow"},
			       {FPE_FLTDIV, "Floating point divide-by-zero"},
			       {FPE_FLTOVF, "Floating point overflow"},
			       {FPE_FLTUND, "Floating point underflow"},
			       {FPE_FLTRES, "Floating point inexact result"},
			       {FPE_FLTINV, "Invalid floating point operation"},
			       {FPE_FLTSUB, "Subscript out of range"},
			       {0, NULL}};
  const s_sicode code_segv[] = {{SEGV_MAPERR, "Address not mapped"},
				{SEGV_ACCERR, "Invalid permissions"},
				{0, NULL}};
  const struct {int signum; const char *name;
    const char *desc; const s_sicode *codes;}
  strs[] = {{SIGFPE, "SIGFPE", sys_siglist[SIGFPE], code_fpe},
	    {SIGILL, "SIGILL", sys_siglist[SIGILL], code_ill},
	    {SIGSEGV, "SIGSEGV", sys_siglist[SIGSEGV], code_segv}};

  context.exec_ctx = ucontext;
  _set_cur_unit();
  for (size_t i = 0; i < sizeof(strs) / sizeof(*strs); ++i)
    if (sig == strs[i].signum) {
      fprintf(stderr, "Crash of execution at: %p\nDue to %s: %s: ",
	      (void*)get_reg(REG_RIP), strs[i].name, strs[i].desc);
      for (size_t j = 0; strs[i].codes[j].desc; ++j) {
	if (info->si_code == strs[i].codes[j].no) {
	    fprintf(stderr, "%s", strs[i].codes[j].desc);
	  if (sig == SIGSEGV)
	    fprintf(stderr, " (%p).\n", info->si_addr);
	  else
	    fprintf(stderr, ".\n");
	  break;
	}
      }
      break;
    }
  ctx_abort_exec();
}

void reset_exec_sighandlers() {
  signal(SIGTRAP, SIG_DFL);
  signal(SIGINT, SIG_DFL);
  signal(SIGFPE, SIG_DFL);
  signal(SIGILL, SIG_DFL);
  signal(SIGSEGV, SIG_DFL);
}

void set_exec_sighandlers() {
  struct sigaction sigact_struct = {0};

  sigact_struct.sa_flags = SA_SIGINFO | SA_ONSTACK;
  sigact_struct.sa_sigaction = &_breakpoint_handler;
  sigaction(SIGTRAP, &sigact_struct, NULL);
  sigaction(SIGINT, &sigact_struct, NULL);
  sigact_struct.sa_sigaction = &_crash_handler;
  sigaction(SIGFPE, &sigact_struct, NULL);
  sigaction(SIGILL, &sigact_struct, NULL);
  sigaction(SIGSEGV, &sigact_struct, NULL);
}

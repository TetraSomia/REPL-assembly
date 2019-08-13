#include <stdio.h>
#include <signal.h>
#include "reg_idx.h"
#include "repl.h"
#include "context_switch.h"
#include "getters.h"

#define TRAP_FLAG_MASK 0x0100

static void _breakpoint_handler(int, siginfo_t*, void*);
static void _trap_flag_handler(int, siginfo_t*, void*);
static void (*_sigtrap_handler)(int, siginfo_t*, void*) = &_breakpoint_handler;
static s_code_instruction *_prev_inst = NULL;
static bool _stepping = false;

static void _step() {
  s_code_unit *unit = unit_find_from_addr((void*)get_reg(REG_RIP));

  if (!unit) { // Stepping into non user code
    if (!_prev_inst)
      fatal_err("Assert failed: _step_closest_user_code(inst!=NULL)\n");
    if (_prev_inst->next) //TODO handle last inst of pointer to function
      *(_prev_inst->next->address) = INT3_OPCODE;
    _prev_inst = NULL;
    _sigtrap_handler = &_breakpoint_handler;
    return;
  }
  printf("Stepping (%p)\n", (void*)get_reg(REG_RIP));
  update_code_unit(unit);
  _stepping = ctx_resume_repl();
  _prev_inst = inst_find_from_addr_all_units((void*)get_reg(REG_RIP));
  if (!_prev_inst)
    fatal_err("Instruction not found from RIP\n"); //TODO get back to repl
  *(_prev_inst->address) = _prev_inst->opcodes[0];
  if (_stepping) {
    *get_reg_ptr(REG_EFL) |= TRAP_FLAG_MASK;
    _sigtrap_handler = &_trap_flag_handler;
  } else {
    _prev_inst = NULL;
    _sigtrap_handler = &_breakpoint_handler;
  }
}

static void _trap_flag_handler(int sig, siginfo_t *info, void *raw_context) {
  (void)sig;
  (void)info;
  context.exec_ctx = (ucontext_t*)raw_context;

  if (!(get_reg(REG_EFL) & TRAP_FLAG_MASK))
    fatal_err("Assert failed: _trap_flag_handler: flag not set\n");
  *get_reg_ptr(REG_EFL) ^= TRAP_FLAG_MASK;
  if (!_prev_inst)
    fatal_err("Assert failed: _trap_flag_handler: _prev_inst == NULL\n");
  if (_prev_inst->breakpoint)
    set_breakpoint(_prev_inst);
  if (_stepping)
    _step();
  else {
    _prev_inst = NULL;
    _sigtrap_handler = &_breakpoint_handler;
  }
  set_exec_sighandlers();
}

static void _breakpoint_handler(int sig, siginfo_t *info, void *raw_context) {
  (void)sig;
  (void)info;
  context.exec_ctx = (ucontext_t*)raw_context;

  *get_reg_ptr(REG_RIP) -= 1;
  printf("Breakpoint hit (%p)\n", (void*)get_reg(REG_RIP));
  update_code_unit(unit_find_from_addr((void*)get_reg(REG_RIP)));
  _stepping = ctx_resume_repl();
  _prev_inst = inst_find_from_addr_all_units((void*)get_reg(REG_RIP));
  if (!_prev_inst)
    fatal_err("Instruction not found from RIP\n"); //TODO get back to repl
  *(_prev_inst->address) = _prev_inst->opcodes[0];
  *get_reg_ptr(REG_EFL) |= TRAP_FLAG_MASK;
  _sigtrap_handler = &_trap_flag_handler;
  set_exec_sighandlers();
}

static void _stop_handler(int sig, siginfo_t *info, void *raw_context) {
  (void)sig;
  (void)info;
  context.exec_ctx = (ucontext_t*)raw_context;
  fprintf(stderr, "Execution stopped manually\n");
  ctx_abort_exec();
}

static void _crash_handler(int sig, siginfo_t *info, void *raw_context) {
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
  const s_sicode code_general[] = {{SI_USER, "Sent by kill()"},
				   {SI_KERNEL, "Sent by the kernel"},
				   {SI_QUEUE, "Sent by sigqueue()"},
				   {SI_TIMER, "POSIX timer expired"},
				   {SI_MESGQ, "POSIX message queue state changed"},
				   {SI_ASYNCIO, "AIO completed"},
				   {SI_SIGIO, "Queued SIGIO"},
				   {SI_TKILL, "Sent by tkill() or tgkill()"},
				   {0, NULL}};
  const struct {int signum; const char *name;
    const char *desc; const s_sicode *codes;}
  strs[] = {{SIGFPE, "SIGFPE", sys_siglist[SIGFPE], code_fpe},
	    {SIGILL, "SIGILL", sys_siglist[SIGILL], code_ill},
	    {SIGSEGV, "SIGSEGV", sys_siglist[SIGSEGV], code_segv}};

  context.exec_ctx = (ucontext_t*)raw_context;
  fflush(stdout);
  update_code_unit(unit_find_from_addr((void*)get_reg(REG_RIP)));
  for (size_t i = 0; i < sizeof(strs) / sizeof(*strs); ++i)
    if (sig == strs[i].signum) {
      fprintf(stderr, "Crash of execution at: %p\nDue to %s: %s: ",
	      (void*)get_reg(REG_RIP), strs[i].name, strs[i].desc);
      bool was_general = false;
      for (size_t j = 0; code_general[j].desc; ++j) {
	if (info->si_code == code_general[j].no) {
	  fprintf(stderr, "%s.\n", code_general[j].desc);
	  was_general = true;
	  break;
	}
      }
      if (was_general)
	break;
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

  sigact_struct.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_RESETHAND;
  sigact_struct.sa_sigaction = _sigtrap_handler;
  sigaction(SIGTRAP, &sigact_struct, NULL);
  sigact_struct.sa_sigaction = &_stop_handler;
  sigaction(SIGINT, &sigact_struct, NULL);
  sigact_struct.sa_sigaction = &_crash_handler;
  sigaction(SIGFPE, &sigact_struct, NULL);
  sigaction(SIGILL, &sigact_struct, NULL);
  sigaction(SIGSEGV, &sigact_struct, NULL);
}

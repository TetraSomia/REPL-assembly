#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "repl.h"
#include "sig_handler.h"
#include "context_switch.h"

s_context context;

static void _free_context() {
  free(context.units[0].name);
  munmap(context.units[0].code, context.units[0].code_max_size);
  rm_instructions(&context.units[0]);
  free(context.units);
  free(context.stack);
}

void _init_signal_handlers() {
  struct sigaction sigact_struct = {0};

  sigact_struct.sa_sigaction = &breakpoint_handler;
  sigact_struct.sa_flags = SA_SIGINFO;
  sigaction(SIGTRAP, &sigact_struct, NULL);
}

void init_context() {
  int pagesize = getpagesize();

  _init_signal_handlers();

  context.stack_size = DEFAULT_STACK_SIZE;
  context.stack = xmalloc(context.stack_size);
  context.exec_ctx = NULL;
  context.units = xmalloc(sizeof(s_code_unit));

  context.units[0].name = strdup("main");
  pagesize = getpagesize();
  context.units[0].code = mmap(NULL, pagesize,
			       PROT_READ | PROT_WRITE | PROT_EXEC,
			       MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
  context.units[0].code_size = 0;
  context.units[0].code_max_size = pagesize;
  context.units[0].insts = NULL;
  add_instruction(&context.units[0], NULL, "mov rax, 0xdeadbeef");
  add_instruction(&context.units[0], context.units[0].insts, "push rax");
  add_instruction(&context.units[0], context.units[0].insts->next, "call rsi");
  add_instruction(&context.units[0], context.units[0].insts->next->next, "pop rax");
  add_instruction(&context.units[0], context.units[0].insts->next->next->next, "ret");
  set_breakpoint(inst_find_from_idx(&context.units[0], 2));
  set_breakpoint(inst_find_from_idx(&context.units[0], 3));

  atexit(&_free_context);
}

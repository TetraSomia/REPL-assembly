#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "repl.h"
#include "context_switch.h"

s_context context;

static void _free_context() {
  for (int i = 0; context.units[i]; ++i) {
    free(context.units[i]->name);
    munmap(context.units[i]->code, context.units[i]->code_max_size);
    rm_instructions(context.units[i]);
    free(context.units[i]);
  }
  free(context.units);

  for (int i = 0; context.dyn_syms && context.dyn_syms[i].sym; ++i) {
    free(context.dyn_syms[i].sym);
    free(context.dyn_syms[i].lib);
  }
  free(context.dyn_syms);

  free(context.stack);
  free(context.sighandler_stack);
}

void init_context() {
  stack_t s;

  context.stack_size = DEFAULT_STACK_SIZE;
  context.stack = xmalloc(context.stack_size);
  memset(context.stack, 0, context.stack_size);

  context.sighandler_stack = xmalloc(context.stack_size);
  s.ss_sp = context.sighandler_stack;
  s.ss_size = context.stack_size;
  s.ss_flags = 0;
  if (sigaltstack(&s, NULL) != 0)
    fatal_libc_err("sigaltstack() failed\n");

  context.exec_ctx = NULL;
  context.cur_unit = add_unit("main");  

  //TODO remove test instructions and breakpoints
  add_instruction(context.cur_unit, NULL, "mov rax, 0xdeadbeef");
  add_instruction(context.cur_unit, inst_find_from_idx(0), "push rax");
  add_instruction(context.cur_unit, inst_find_from_idx(1), "call rsi");
  add_instruction(context.cur_unit, inst_find_from_idx(2), "pop rax");
  set_breakpoint(inst_find_from_idx(2));
  set_breakpoint(inst_find_from_idx(3));

  context.dyn_syms = NULL;
  
  atexit(&_free_context);
}

#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "repl.h"
#include "context_switch.h"

s_context context;

static void _free_context() {
  free(context.units[0].name);
  munmap(context.units[0].code, context.units[0].code_max_size);
  rm_instructions(&context.units[0]);
  free(context.units);
  free(context.stack);
}

void init_context() {
  int pagesize = getpagesize();

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
  //add_instruction(&context.units[0], NULL, "loop:\njmp loop"); //infinite loop
  //add_instruction(&context.units[0], NULL, "mov rax, [rax]"); //SIGSEGV
  //add_instruction(&context.units[0], NULL, "ud2"); //SIGILL
  //add_instruction(&context.units[0], NULL, "div rax"); //SIGFPE
  add_instruction(&context.units[0], context.units[0].insts, "push rax");
  add_instruction(&context.units[0], context.units[0].insts->next, "call rsi");
  add_instruction(&context.units[0], context.units[0].insts->next->next, "pop rax");
  add_instruction(&context.units[0], context.units[0].insts->next->next->next, "ret");
  set_breakpoint(inst_find_from_idx(&context.units[0], 2));
  set_breakpoint(inst_find_from_idx(&context.units[0], 3));

  atexit(&_free_context);
}

#include <ucontext.h>
#include <string.h>
#include <stdio.h> //tmp
#include "repl.h"

static ucontext_t _repl_ctx, _exec_ctx;//, _sighandler_ctx;

/*
** "Signal number 0 is reserved for use as kill(pid, 0)"
** from <bits/signum-generic.h>
** So we can use 0 to indicate exit_success
*/
static int _status = 0;
static bool _ctx_running = false;

void ctx_run_unit(s_code_unit *unit) {
  //TODO if (_ctx_running == true)
  _status = 0;
  _ctx_running = true;
  memset(context.stack, 0, context.stack_size);
  getcontext(&_exec_ctx);
  _exec_ctx.uc_link = &_repl_ctx;
  _exec_ctx.uc_stack.ss_sp = context.stack;
  _exec_ctx.uc_stack.ss_size = context.stack_size;
  makecontext(&_exec_ctx, (void (*)())unit->code, 2, "hello from asm", &puts);
  printf("launching asm\n");
  swapcontext(&_repl_ctx, &_exec_ctx);
  printf("returning from asm\n");
}

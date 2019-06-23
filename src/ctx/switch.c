#include <signal.h>
#include <string.h>
#include <stdio.h> //TODO tmp
#include "repl.h"
#include "context_switch.h"

static ucontext_t _repl_ctx, _exec_ctx, _breakpoint_ctx;

/*
** "Signal number 0 is reserved for use as kill(pid, 0)"
** from <bits/signum-generic.h>
** So we can use 0 to indicate exit_success
*/

static bool _ctx_running = false;
static bool _ctx_stopped = false;

void ctx_resume_repl() {
  _ctx_stopped = true;
  if (swapcontext(&_breakpoint_ctx, &_repl_ctx) != 0)
    fatal_libc_err("swapcontext(&_breakpoint_ctx, &_repl_ctx) failed\n");
  _ctx_stopped = false;
}

void ctx_resume_exec() {
  if (!_ctx_running)
    return;
  if (setcontext(&_breakpoint_ctx) != 0)
    fatal_libc_err("setcontext(&_breakpoint_ctx) failed\n");
}

ucontext_t *ctx_get_repl_ctx() {
  return &_repl_ctx;
}

void ctx_handle_ctx_update() {
  if (!_ctx_running)
    return;
  if (!_ctx_stopped) {
    puts("returning from asm");
    _ctx_running = false;
  }
}

void ctx_run_unit(s_code_unit *unit) {
  if (_ctx_running == true)
    return;
  _ctx_running = true;
  memset(context.stack, 0, context.stack_size); //cause valgrind errors
  getcontext(&_exec_ctx);
  _exec_ctx.uc_link = &_repl_ctx;
  _exec_ctx.uc_stack.ss_sp = context.stack;
  _exec_ctx.uc_stack.ss_size = context.stack_size;
  sigemptyset(&_exec_ctx.uc_sigmask);
  makecontext(&_exec_ctx, (void (*)())unit->code, 2, "hello from asm", &puts);
  printf("launching asm\n");
  if (setcontext(&_exec_ctx) != 0)
    fatal_libc_err("setcontext(&_exec_ctx) failed\n");
}

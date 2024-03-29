#include <signal.h>
#include <string.h>
#include <stdio.h>
#include "repl.h"
#include "getters.h"
#include "context_switch.h"

static ucontext_t _repl_ctx, _exec_ctx;
static bool _ctx_running = false;
static bool _ctx_stopped = false;
static bool _ctx_aborted = false;
static bool _step = false;

bool ctx_resume_repl() {
  _ctx_stopped = true;
  if (swapcontext(&_exec_ctx, &_repl_ctx) != 0)
    fatal_libc_err("swapcontext(&_exec_ctx, &_repl_ctx) failed\n");
  _ctx_stopped = false;
  return _step;
}

void ctx_resume_exec(bool step) {
  if (!_ctx_running)
    return;
  _step = step;
  if (setcontext(&_exec_ctx) != 0)
    fatal_libc_err("setcontext(&_exec_ctx) failed\n");
}

void ctx_abort_exec() {
  if (_ctx_running)
    _ctx_aborted = true;
  if (setcontext(&_repl_ctx) != 0)
    fatal_libc_err("setcontext(&_repl_ctx) failed\n");
}

ucontext_t *ctx_get_repl_ctx() {
  return &_repl_ctx;
}

void ctx_handle_ctx_update() {
  reset_exec_sighandlers();
  if (!_ctx_running || (_ctx_stopped && !_ctx_aborted))
    return;
  if (!_ctx_aborted)
    puts("Code exited normally");
  else
    _ctx_aborted = false;
  _ctx_running = false;
  _ctx_stopped = false;
  context.exec_ctx = NULL;
}

void ctx_run_unit(s_code_unit *unit) {
  if (_ctx_running == true)
    return;
  _ctx_running = true;
  memset(context.stack, 0, context.stack_size); //causes valgrind errors
  getcontext(&_exec_ctx);
  _exec_ctx.uc_link = &_repl_ctx;
  _exec_ctx.uc_stack.ss_sp = context.stack;
  _exec_ctx.uc_stack.ss_size = context.stack_size;
  sigemptyset(&_exec_ctx.uc_sigmask);
  makecontext(&_exec_ctx, (void (*)())unit->code,
	      2, "hello from code", &puts); //2 arguments for demo purposes
  set_exec_sighandlers();
  puts("Starting code execution");
  if (setcontext(&_exec_ctx) != 0)
    fatal_libc_err("setcontext(&_exec_ctx) failed\n");
}

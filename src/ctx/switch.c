#include <ucontext.h>
#include <string.h>
#include <stdio.h> //tmp
#include <signal.h> //tmp
#include "repl.h"

static ucontext_t _repl_ctx, _rununit_ctx, _exec_ctx, _breakpoint_ctx;

/*
** "Signal number 0 is reserved for use as kill(pid, 0)"
** from <bits/signum-generic.h>
** So we can use 0 to indicate exit_success
*/
static int _status = 0;
static bool _ctx_running = false;

static void *_run_unit_callback_stack = NULL;

void ctx_resume_repl() {
  puts("a");
  if (swapcontext(&_breakpoint_ctx, &_repl_ctx) != 0)
    fatal_libc_err("swapcontext(&_breakpoint_ctx, &_repl_ctx) failed\n");
}

void ctx_resume_exec() {
  if (!_ctx_running)
    return;
  if (setcontext(&_breakpoint_ctx) != 0)
    fatal_libc_err("setcontext(&_breakpoint_ctx) failed\n");
}

void ctx_set_repl_context() {
  if (getcontext(&_repl_ctx) != 0)
    fatal_libc_err("getcontext(&_repl_ctx) failed\n");
  puts("b");
}

static void _run_unit_callback() {
  printf("returning from asm\n");
  _ctx_running = false;  
}

void ctx_run_unit(s_code_unit *unit) {
  if (_ctx_running == true)
    return;
  _status = 0;
  _ctx_running = true;

  if (_run_unit_callback_stack == NULL)
    _run_unit_callback_stack = xmalloc(SIGSTKSZ);
  memset(_run_unit_callback_stack, 0, SIGSTKSZ);
  getcontext(&_rununit_ctx);
  _rununit_ctx.uc_link = &_repl_ctx;
  _rununit_ctx.uc_stack.ss_sp = _run_unit_callback_stack;
  _rununit_ctx.uc_stack.ss_size = SIGSTKSZ;
  makecontext(&_rununit_ctx, (void (*)())_run_unit_callback, 0);
  
  memset(context.stack, 0, context.stack_size);
  getcontext(&_exec_ctx);
  _exec_ctx.uc_link = &_rununit_ctx;
  _exec_ctx.uc_stack.ss_sp = context.stack;
  _exec_ctx.uc_stack.ss_size = context.stack_size;
  makecontext(&_exec_ctx, (void (*)())unit->code, 2, "hello from asm", &puts);
  printf("launching asm\n");
  if (setcontext(&_exec_ctx) != 0)
    fatal_libc_err("setcontext(&_exec_ctx) failed\n");
}

#pragma once

#include <ucontext.h>
#include "code_unit.h"

void ctx_run_unit(s_code_unit *unit);
void ctx_handle_ctx_update();
void ctx_resume_exec();
void ctx_resume_repl();
ucontext_t *ctx_get_repl_ctx();

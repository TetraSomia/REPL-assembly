#pragma once

#include "code_unit.h"

void ctx_run_unit(s_code_unit *unit);
void ctx_set_repl_context();
void ctx_resume_exec();
void ctx_resume_repl();

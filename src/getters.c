#include "getters.h"
#include "repl.h"

uint64_t get_reg(e_register reg) {
  if (!context.exec_ctx)
    return 0;
  return context.exec_ctx->uc_mcontext.gregs[reg];
}

#include "getters.h"
#include "repl.h"

uint64_t get_reg(e_register reg) {
  if (!context.exec_ctx)
    return 0;
  return context.exec_ctx->uc_mcontext.gregs[reg];
}

uint64_t *get_reg_ptr(e_register reg) {
  if (!context.exec_ctx)
    return NULL;
  return (uint64_t*)&context.exec_ctx->uc_mcontext.gregs[reg];
}

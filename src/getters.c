#include <stdio.h>
#include <string.h>
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

bool is_running() {
  if (!context.exec_ctx)
    return false;
  return true;
}

e_parsed_type get_addr_or_idx(const char *s, u_parsed_val *v) {
  e_parsed_type t = PARSED_ERROR;

  if (strncmp(s, "0x", 2) == 0) {
    if (strlen(s) > 2 && sscanf(s, "%lx", &v->addr) == 1)
      t = PARSED_ADDR;
  } else if (sscanf(s, "%d", &v->idx) == 1)
    t = PARSED_IDX;
  return t;
}

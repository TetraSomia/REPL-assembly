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

int parse_str_to_val(const char *s, u_parsed_val *v) {

  v->type = PARSED_ERROR;
  if (strncmp(s, "0x", 2) == 0) {
    if (strlen(s) > 2 && sscanf(s, "%lx", &v->addr) == 1)
      v->type = PARSED_ADDR;
  } else if (sscanf(s, "%d", &v->idx) == 1)
    v->type = PARSED_IDX;
  else {
    for (e_register r = REG_FIRST; r < REG_LAST; ++r)
      if (strcasecmp(reg_names[r], s) == 0) {
	v->reg = r;
	v->type = PARSED_REG;
	break;
      }
  }
  return v->type == PARSED_ERROR ? 1 : 0;
}

s_code_instruction *get_inst_from_parsing(const u_parsed_val *v) {
  s_code_instruction *inst = NULL;

  switch (v->type) {
  case PARSED_ADDR:
    if ((inst = inst_find_from_addr((void*)v->addr)) == NULL)
      p_error("No instruction matches address 0x%lx"
                     " in the current unit\n", v->addr);
    break;
  case PARSED_IDX:
    if ((inst = inst_find_from_idx(v->idx)) == NULL)
      p_error("No instruction matches index %d in the current unit\n", v->idx);
    break;
  default:
    p_error("Instruction parsing failed (must be an index or address)\n");
    break;
  }
  return inst;
}

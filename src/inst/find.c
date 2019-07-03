#include "repl.h"

s_code_instruction *inst_find_from_addr(const void *addr) {
  for (int i = 0; context.units[i].name; ++i)
    for (s_code_instruction *inst = context.units[i].insts;
	 inst; inst = inst->next)
      if (inst->address == addr)
	return inst;
  return NULL;
}

s_code_instruction *inst_find_from_unit_and_idx(s_code_unit *unit, int idx) {
  for (s_code_instruction *inst = unit->insts; inst; inst = inst->next)
    if (inst->index == idx)
      return inst;
  return NULL;
}

s_code_instruction *inst_find_from_idx(int idx) {
  for (s_code_instruction *inst = context.cur_unit->insts;
       inst; inst = inst->next)
    if (inst->index == idx)
      return inst;
  return NULL;
}

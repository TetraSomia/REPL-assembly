#include <stdlib.h>
#include "code_unit.h"
#include "inst_internal.h"

static void _free_inst_content_and_self(s_code_instruction *inst) {
  free(inst->str_gen);
  free(inst->str_sym);
  free(inst->str_input);
  free(inst->label);
  free(inst);
}

void rm_instruction(s_code_unit *unit, s_code_instruction *inst) {
  if (inst->prev)
    inst->prev->next = inst->next;
  else
    unit->insts = inst->next;
  if (inst->next)
    inst->next->prev = inst->prev;
  _free_inst_content_and_self(inst);
  if (commit_code(unit) != 0)
    fatal_err("Assert failed: removing an instruction shouldn't fail\n");
}

void rm_instructions(s_code_unit *code_unit) {
  s_code_instruction *inst;
  s_code_instruction *next_inst;

  code_unit->code_size = 0;
  inst = code_unit->insts;
  code_unit->insts = NULL;
  while (inst) {
    next_inst = inst->next;
    _free_inst_content_and_self(inst);
    inst = next_inst;
  }
}

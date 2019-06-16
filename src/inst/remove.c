#include <stdlib.h>
#include "code_unit.h"

static void _free_inst_content_and_self(s_code_instrution *inst) {
  free(inst->str_gen);
  free(inst->str_input);
  if (inst->label)
    free(inst->label);
  free(inst);
}

int rm_instruction(s_code_instrution *inst) {
  (void)inst;
  //TODO implement
  return 0;
}

void rm_instructions(s_code_unit *code_unit) {
  s_code_instrution *inst;
  s_code_instrution *next_inst;

  code_unit->code_size = 0;
  inst = code_unit->insts;
  while (inst) {
    next_inst = inst->next;
    _free_inst_content_and_self(inst);
    inst = next_inst;
  }
}

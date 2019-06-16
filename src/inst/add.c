#include "code_unit.h"

static s_code_instrution *_create_inst(const s_code_instrution *prev_inst,
				       s_code_instrution *next_inst,
				       const char *inst) {
  s_code_instrution *new;

  new = xmalloc(sizeof(s_code_instrution));
  new->size = 0;
  new->str_gen = xstrdup("");
  new->str_input = xstrdup(inst);
  new->label = NULL;
  new->address = NULL;
  new->index = prev_inst ? prev_inst->index + 1 : 0;
  new->next = next_inst;
  return new;
}

int add_instruction(s_code_unit *unit, s_code_instrution *prev_inst,
		    const char *inst) {
  s_code_instrution *next_inst;
  s_code_instrution *new_inst;

  next_inst = prev_inst ? prev_inst->next : unit->insts;
  new_inst = _create_inst(prev_inst, next_inst, inst);
  if (prev_inst)
    prev_inst->next = new_inst;
  else
    unit->insts = new_inst;
  //commit changes
  return 0;
}

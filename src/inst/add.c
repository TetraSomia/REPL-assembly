#include "code_unit.h"
#include "inst_internal.h"

static s_code_instruction *_create_inst(const s_code_instruction *prev_inst,
				       s_code_instruction *next_inst,
				       const char *inst) {
  s_code_instruction *new;

  new = xmalloc(sizeof(s_code_instruction));
  new->size = 0;
  new->str_gen = xstrdup("");
  new->str_input = xstrdup(inst);
  new->label = NULL;
  new->address = NULL;
  new->index = prev_inst ? prev_inst->index + 1 : 0;
  new->next = next_inst;
  return new;
}

int add_instruction(s_code_unit *unit, s_code_instruction *prev_inst,
		    const char *inst) {
  s_code_instruction *next_inst;
  s_code_instruction *new_inst;
  int err;

  next_inst = prev_inst ? prev_inst->next : unit->insts;
  new_inst = _create_inst(prev_inst, next_inst, inst);
  if (prev_inst)
    prev_inst->next = new_inst;
  else
    unit->insts = new_inst;
  for (s_code_instruction *i = new_inst; i->next ; i = i->next)
    i->next->index = i->index + 1;
  err = commit_code(unit);
  // TODO if err, unlink ?
  return err;
}

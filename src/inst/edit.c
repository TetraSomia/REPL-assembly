#include <stdlib.h>
#include "code_unit.h"
#include "inst_internal.h"

int edit_instruction(s_code_unit *unit, s_code_instruction *inst,
		     const char *inst_str) {
  char *old_str_input;
  char *old_str_sym;
  int err;

  assert_single_line(inst_str);
  old_str_input = inst->str_input;
  old_str_sym = inst->str_sym;
  inst->str_input = xstrdup(inst_str);
  inst->str_sym = NULL;
  err = commit_code(unit);
  if (err) {
    free(inst->str_input);
    free(inst->str_sym);
    inst->str_input = old_str_input;
    inst->str_sym = old_str_sym;
  } else {
    free(old_str_input);
    free(old_str_sym);
  }
  return err;
}

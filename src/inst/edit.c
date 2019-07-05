#include <stdlib.h>
#include "code_unit.h"
#include "inst_internal.h"

int edit_instruction(s_code_unit *unit, s_code_instruction *inst,
		     const char *inst_str) {
  char *old_str_input;
  int err;

  assert_single_line(inst_str);
  old_str_input = inst->str_input;
  inst->str_input = xstrdup(inst_str);
  err = commit_code(unit);
  if (err) {
    free(inst->str_input);
    inst->str_input = old_str_input;
  } else
    free(old_str_input);
  return err;
}

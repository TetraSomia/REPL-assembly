#include <stdlib.h>
#include "repl.h"
#include "inst_internal.h"

int set_label(s_code_instruction *inst, const char *label) {
  char *old = inst->label;
  int err;

  inst->label = (char*)label;
  err = commit_code(context.cur_unit);
  if (err) {
    inst->label = old;
    return 1;
  }
  if (inst->label)
    inst->label = xstrdup(inst->label);
  free(old);
  return 0;
}

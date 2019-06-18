#include <stdlib.h>
#include "inst_internal.h"

int commit_code(s_code_unit *unit) {
  s_parsed_inst *parsed_insts;

  parsed_insts = assemble(unit);
  if (parsed_insts == NULL)
    return 1;
  for (s_parsed_inst *inst = parsed_insts; inst->inst; inst++)
    free(inst->inst);
  free(parsed_insts);
  return 0;
}

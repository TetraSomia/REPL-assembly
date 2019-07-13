#include "repl.h"

static void _parse_code_units(s_code_instruction *inst) {
  inst->str_sym = xstrdup(inst->str_input);
}

int parse_symbols(s_code_unit *unit) {
  for (s_code_instruction *i = unit->insts; i; i = i->next) {
    if (i->str_sym)
      continue;
    _parse_code_units(i);    
  }
  return 0;
}

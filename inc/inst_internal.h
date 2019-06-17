#pragma once

#include "code_unit.h"

typedef struct {
  int offset;
  char *inst;
} s_parsed_inst;

int commit_code(s_code_unit *unit);

s_parsed_inst *assemble(s_code_unit *unit);

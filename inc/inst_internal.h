#pragma once

#include "code_unit.h"

typedef struct {
  int offset;
  char *inst;
} s_parsed_inst;

int commit_code(s_code_unit *unit);
void parse_nasm_error(s_code_unit *unit, const char *err);

/*
** return NULL on NASM parsing error
*/

s_parsed_inst *assemble(s_code_unit *unit);

/*
** Dump functions
*/

void dump_code_instruction(s_code_instruction *inst);
void dump_code_unit(s_code_unit *unit);

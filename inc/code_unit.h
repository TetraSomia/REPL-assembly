#pragma once

#include "utils.h"

#define MAX_INST_SIZE 15

typedef struct _s_code_instruction {
  uint8_t opcodes[MAX_INST_SIZE];
  size_t size;
  char *str_gen;
  char *str_input;
  char *label;
  uint8_t *address;
  int index;
  bool breakpoint;
  struct _s_code_instruction *next;
} s_code_instruction;

typedef	struct {
  char *name;
  uint8_t *code;
  size_t code_size;
  s_code_instruction *insts;
} s_code_unit;

int add_instruction(s_code_unit *unit, s_code_instruction *prev_inst,
		    const char *inst);
int rm_instruction(s_code_instruction *inst);
void rm_instructions(s_code_unit *code_unit);

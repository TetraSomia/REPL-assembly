#pragma once

#include "utils.h"

#define MAX_INST_SIZE 15

typedef struct _s_code_instrution {
  uint8_t opcodes[MAX_INST_SIZE];
  size_t inst_size;
  char *inst_name;
  char *label;
  uint8_t *address;
  bool breakpoint;
  struct _s_code_instrution *next;
} s_code_instrution;

typedef	struct {
  char *name;
  uint8_t *code;
  size_t code_size;
  s_code_instrution *insts;
} s_code_unit;

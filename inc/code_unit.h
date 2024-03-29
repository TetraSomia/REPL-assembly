#pragma once

#include "utils.h"

#define MAX_INST_SIZE 15
#define INT3_OPCODE 0xCC

typedef struct _s_code_instruction {
  uint8_t opcodes[MAX_INST_SIZE];
  size_t size;
  char *str_gen;
  char *str_sym;
  char *str_input;
  char *label;
  uint8_t *address;
  int index;
  bool breakpoint;
  struct _s_code_instruction *prev;
  struct _s_code_instruction *next;
} s_code_instruction;

typedef	struct {
  char *name;
  uint8_t *code;
  size_t code_size;
  size_t code_max_size;
  s_code_instruction *insts;
} s_code_unit;

s_code_instruction *add_instruction(s_code_unit *unit,
				    s_code_instruction *prev_inst,
				    const char *inst);
int edit_instruction(s_code_unit *unit, s_code_instruction *inst,
		     const char *inst_str);
void rm_instruction(s_code_unit *unit, s_code_instruction *inst);
void rm_instructions(s_code_unit *code_unit);

s_code_instruction *inst_find_from_addr(const void *addr);
s_code_instruction *inst_find_from_addr_all_units(const void *addr);
s_code_instruction *inst_find_from_idx(int idx);
s_code_instruction *inst_find_from_unit_and_idx(s_code_unit *unit, int idx);

s_code_unit *unit_find_from_addr(const void *addr);
s_code_unit *unit_find_from_name(const char *name);
s_code_unit *add_unit(char *name);
void remove_unit(s_code_unit *unit);
void update_code_unit(s_code_unit *unit);

void set_breakpoint(s_code_instruction *inst);
void reset_breakpoint(s_code_instruction *inst);

int set_label(s_code_instruction *inst, const char *label);

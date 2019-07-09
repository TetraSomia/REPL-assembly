#pragma once

#include "utils.h"
#include "reg_idx.h"
#include "code_unit.h"

uint64_t get_reg(e_register reg);
uint64_t *get_reg_ptr(e_register reg);
bool is_running();

/*
** Parsing function to get an address or an index from a string.
** Fills the 'v' struct with the parsed output and its type.
** Returns 1 on error, 0 otherwise.
*/

typedef enum {
	      PARSED_ERROR,
	      PARSED_ADDR,
	      PARSED_IDX,
	      PARSED_REG
} e_parsed_type;

typedef struct {
  e_parsed_type type;
  union {
    uint64_t addr;
    int idx;
    e_register reg;
  };
} u_parsed_val;

int parse_str_to_val(const char *s, u_parsed_val *v);
s_code_instruction *get_inst_from_parsing(const u_parsed_val *v);
uint64_t *get_ptr_from_parsing(const u_parsed_val *v);

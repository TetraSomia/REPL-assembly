#pragma once

#include "utils.h"
#include "reg_idx.h"

uint64_t get_reg(e_register reg);
uint64_t *get_reg_ptr(e_register reg);
bool is_running();

/*
** Parsing function to get an address or an index from a string.
** Returns the type of the ouput (address, index or error).
** Fills the 'v' union with the parsed output.
*/

typedef union {
  uint64_t addr;
  int idx;
} u_parsed_val;

typedef enum {
	      PARSED_ERROR,
	      PARSED_ADDR,
	      PARSED_IDX
} e_parsed_type;

e_parsed_type get_addr_or_idx(const char *s, u_parsed_val *v);

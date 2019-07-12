#include <stdio.h>
#include "repl.h"
#include "getters.h"
#include "print.h"

static inline void _set_value(void *addr, char wordsize, uint64_t data) {
  switch (wordsize) {
  case 1:
    *((uint8_t*)addr) = data;
    break;
  case 2:
    *((uint16_t*)addr) = data;
    break;
  case 4:
    *((uint32_t*)addr) = data;
    break;
  case 8:
    *((uint64_t*)addr) = data;
    break;
  }
}

int cmd_set(int ac, char* const *av) {
  char *deref_val;
  int nbr_deref;
  u_parsed_val data;
  char wordsize = 8;
  uint64_t value;
  int value_idx = 1;

  if (ac < 2)
    return p_error("Please specify a location (register or address)"
		   " and a value\n");
  deref_val = parse_dereferencing(av[0], &nbr_deref);
  if (parse_str_to_val(deref_val, &data) != 0 || data.type == PARSED_IDX)
    return p_error("\'%s\' cannot be parsed into a register or address\n",
		   deref_val);
  if (data.type == PARSED_REG && !is_running())
    return p_error("Register parsing failed: No code is running\n");
  do_dereferencing(&data, nbr_deref);
  if (ac > 2) {
    if ((wordsize = print_parse_wordsize(av[1])) == 0)
      return p_error("\'%s\' cannot be parsed into a valid "
		     "word size (1/2/4/8)\n", av[1]);
    value_idx++;
  }
  if (sscanf(av[value_idx], "%lx", &value) != 1)
    return p_error("\'%s\' cannot be parsed into a hexadecimal value\n",
		   av[value_idx]);
  _set_value(get_ptr_from_parsing(&data), wordsize, value);
  return 0;
}

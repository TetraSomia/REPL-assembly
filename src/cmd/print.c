#include <stdio.h>
#include <string.h>
#include "repl.h"
#include "getters.h"
#include "print.h"

static int _print_regs() {
  if (!is_running())
    return p_error("Cannot print registers: code isn't running\n");
  puts("Printing registers content:");
  for (e_register r = REG_FIRST; r < REG_LAST; ++r)
    print_reg(r, PRINT_FMT_HEX, 8, 1);
  return 0;
}

int cmd_print(int ac, char* const *av) {
  char *deref_val;
  int nbr_deref;
  u_parsed_val data;
  e_print_fmt fmt = PRINT_FMT_HEX;
  char wordsize = 8;
  size_t iter = 1;

  if (ac < 1)
    return p_error("Please specify a location (register(s) or address)\n");
  if (strcmp(av[0], "regs") == 0)
    return _print_regs();
  deref_val = parse_dereferencing(av[0], &nbr_deref);
  if (parse_str_to_val(deref_val, &data) != 0 || data.type == PARSED_IDX)
    return p_error("\'%s\' cannot be parsed into a register or address\n",
		   deref_val);
  if (data.type == PARSED_REG && !is_running())
    return p_error("Register parsing failed: No code is running\n");
  do_dereferencing(&data, nbr_deref);
  if (ac > 1)
    if ((fmt = print_parse_format(av[1])) == PRINT_FMT_ERR)
      return p_error("\'%s\' cannot be parsed into a valid format (s/x/d)\n",
		     av[1]);
  if (fmt == PRINT_FMT_STR && data.type == PARSED_REG)
      return p_error("Cannot use string format for register\n");
  if (ac > 2) {
    if (fmt == PRINT_FMT_STR)
      return p_error("Parameters 3 and 4 do not apply to string format\n");
    if ((wordsize = print_parse_wordsize(av[2])) == 0)
      return p_error("\'%s\' cannot be parsed into a valid "
		     "word size (1/2/4/8)\n", av[2]);
  }
  if (ac > 3 && sscanf(av[3], "%ld", &iter) != 1)
    return p_error("\'%s\' cannot be parsed into an integer\n", av[3]);
  if (iter * wordsize > 8 && data.type == PARSED_REG)
    return p_error("You cannot print more than 8 bytes per register\n");
  if (data.type == PARSED_REG)
    return print_reg(data.reg, fmt, wordsize, iter);
  return print_addr((uint64_t*)data.addr, fmt, wordsize, iter);
}

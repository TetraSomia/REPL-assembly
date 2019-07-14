#include <string.h>
#include <stdio.h>
#include "repl.h"
#include "getters.h"

int cmd_breakpoint(int ac, char* const *av) {
  u_parsed_val inst_v;
  s_code_instruction *inst;

  if (ac < 1)
    return p_error("Please specify an instruction (index or address)\n");
  if (ac > 1 && strcmp(av[1], "!") != 0)
    return p_error("\'%s\' must be \'!\' or shouldn't be provided\n", av[1]);
  if (parse_str_to_val(av[0], &inst_v) != 0)
    return p_error("\'%s\' cannot be parsed into an index or address\n", av[0]);
  if ((inst = get_inst_from_parsing(&inst_v)) == NULL)
    return 1;
  if (ac == 1) {
    set_breakpoint(inst);
    puts("Breakpoint set.");
  }
  else {
    reset_breakpoint(inst);
    puts("Breakpoint removed.");
  }
  return 0;
}

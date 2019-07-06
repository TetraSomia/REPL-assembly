#include <string.h>
#include <stdio.h>
#include <readline/readline.h>
#include <stdlib.h>
#include "repl.h"
#include "getters.h"

static int _read_label(s_code_instruction *inst) {
  char *line;

  puts("Type the label name:");
  line = readline("> ");
  if (line == NULL)
    return p_warning("Aborting instruction insertion\n");
  //TODO check bad character
  set_label(inst, line);
  free(line);
  return 0;
}

int cmd_label(int ac, char **av) {
  u_parsed_val inst_v;
  s_code_instruction *inst;

  if (ac < 1)
    return p_error("Please specify an instruction (index or address)\n");
  if (ac > 1 && strcmp(av[1], "!") != 0)    
    return p_error("\'%s\' must be \'!\' or shouldn't be provided\n", av[1]);
  if (get_addr_or_idx(av[0], &inst_v) != 0)
    return p_error("\'%s\' cannot be parsed into an index or address\n", av[0]);
  if ((inst = get_inst_from_parsing(&inst_v)) == NULL)
    return 1;
  if (ac == 1)
    return _read_label(inst);
  if (!reset_label(inst))
    p_warning("No label to remove\n");
  else
    puts("Label removed");
  return 0;
}

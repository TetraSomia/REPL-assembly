#include <string.h>
#include <stdio.h>
#include <readline/readline.h>
#include <stdlib.h>
#include "repl.h"
#include "getters.h"

static int _read_label(s_code_instruction *inst) {
  char *line;
  int err = 0;

  puts("Type the label name:");
  line = readline("> ");
  if (line == NULL)
    return p_warning("Aborting instruction insertion\n");
  str_clear_sep(line);
  if (str_is_empty(line) || str_is_comment(line))
    err = p_error("Cannot insert empty or commented label\n");
  if (!err)
    err = set_label(inst, line);
  free(line);
  return err;
}

static int _rm_label(s_code_instruction *inst) {
  int err;

  if (!inst->label)
    return p_warning("No label to remove\n");
  err = set_label(inst, NULL);
  if (err)
    return 1;
  puts("Label removed");
  return 0;
}

int cmd_label(int ac, char **av) {
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
  if (ac == 1)
    return _read_label(inst);
  else
    return _rm_label(inst);
}

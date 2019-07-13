#include <stdio.h>
#include <readline/readline.h>
#include <stdlib.h>
#include "repl.h"

static void _print_units() {
  puts("Listing code units:");
  for (int i = 0; context.units[i]; ++i)
    printf("%s %s\n", context.units[i] == context.cur_unit ? "=>" : "  ",
	   context.units[i]->name);
}

static s_code_unit *_ask_create_unit(char *name) {
  s_code_unit *unit = NULL;
  char *line;

  line = readline("Unit not found, would you like to create it (Y/n)? ");
  if (line == NULL)
    return NULL;
  if (str_is_empty(line) || strcasecmp(line, "y") == 0)
    unit = add_unit(name);
  free(line);
  return unit;
}

int cmd_unit(int ac, char* const *av) {
  s_code_unit *unit;

  if (ac < 1) {
    _print_units();
    return 0;
  }
  unit = unit_find_from_name(av[0]);
  if (ac > 1) {
    if (strcmp(av[1], "!") != 0)
      return p_error("'%s' must be \'!\' or shouldn't be provided\n", av[1]);
    if (!unit)
      return p_error("'%s' unit not found\n", av[0]);
    if (context.units[1] == NULL)
      return p_error("Cannot remove the last unit\n");
    remove_unit(unit);
    puts("Code unit removed.");
  } else {
    if (!unit)
      if (!(unit = _ask_create_unit(av[0])))
	return 1;
    update_code_unit(unit);
  }
  return 0;
}

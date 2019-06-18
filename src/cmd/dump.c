#include "inst_internal.h"
#include "repl.h"

int cmd_dump(int ac, char **av) {
  (void)ac;
  (void)av;
  dump_code_unit(&context.units[0]);
  return 0;
}

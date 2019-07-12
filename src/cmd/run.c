#include <stdio.h>
#include "context_switch.h"
#include "repl.h"

int cmd_run(int ac, char* const *av) {
  (void)ac;
  (void)av;
  //TODO run unit other than main
  ctx_run_unit(unit_find_from_name("main"));
  puts("Warning: code is already running");
  return 0;
}

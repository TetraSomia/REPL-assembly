#include <stdio.h>
#include "context_switch.h"
#include "repl.h"

int cmd_run(int ac, char* const *av) {
  (void)ac;
  (void)av;
  ctx_run_unit(&context.units[0]);
  puts("Warning: code is already running");
  return 0;
}

#include "context_switch.h"
#include "repl.h"

int cmd_run(int ac, char **av) {
  (void)ac;
  (void)av;
  ctx_run_unit(&context.units[0]);
  return 0;
}

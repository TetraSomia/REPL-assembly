#include <stdio.h>
#include "context_switch.h"
#include "repl.h"

int cmd_step(int ac, char* const *av) {
  (void)ac;
  (void)av;
  ctx_resume_exec(true);
  puts("Warning: No code is currently running");
  return 0;
}

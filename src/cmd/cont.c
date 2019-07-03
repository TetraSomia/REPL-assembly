#include <stdio.h>
#include "context_switch.h"
#include "repl.h"

int cmd_cont(int ac, char **av) {
  (void)ac;
  (void)av;
  ctx_resume_exec();
  puts("Warning: No code is currently running");
  return 0;
}

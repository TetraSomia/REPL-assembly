#include <stdio.h>
#include "repl.h"
#include "commands.h"

int main(int ac, char **av)
{
  (void)av;
  if (ac > 1) {
    cmd_help(0, NULL);
    return 0;
  }
  init_context();
  repl();
  return 0;
}

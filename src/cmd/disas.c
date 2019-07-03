#include <stdio.h>
#include <string.h>
#include "repl.h"
#include "getters.h"

static void _disas_unit(s_code_unit *unit) {
  void *rip = NULL;

  if (is_running())
    rip = (void*)get_reg(REG_RIP);
  for (s_code_instruction *i = unit->insts; i; i = i->next) {
    if (i->label)
      printf("%s:\n", i->label);
    char prefix[3] = "  ";
    if (rip == i->address)
      strcpy(prefix, "=>");
    else if (i->breakpoint)
      strcpy(prefix, "* ");
    printf("%d: %s %p\t%s\n", i->index, prefix, i->address, i->str_gen);
  }
}

int cmd_disas(int ac, char **av) {
  (void)ac;
  (void)av;
  //TODO make multi code_unit compatible
  _disas_unit(&context.units[0]);
  return 0;
}

#include <stdio.h>
#include <string.h>
#include "repl.h"
#include "getters.h"

static void _disas_unit(s_code_unit *unit) {
  void *rip = NULL;

  if (is_running())
    rip = (void*)get_reg(REG_RIP);
  for (s_code_instruction *i = unit->insts; i; i = i->next) {
    char prefix[3] = "  ";
    if (rip == i->address)
      strcpy(prefix, "=>");
    else if (i->breakpoint)
      strcpy(prefix, "* ");
    if (i->label)
      printf("%d:\t   %s:\n\t%s %p\t%s\n", i->index, i->label, prefix,
	     i->address, i->str_gen);
    else
      printf("%d:\t%s %p\t%s\n", i->index, prefix, i->address, i->str_gen);
  }
}

int cmd_disas(int ac, char **av) {
  (void)ac;
  (void)av;
  //TODO make multi code_unit compatible
  printf("%s:\n", context.cur_unit->name);
  _disas_unit(context.cur_unit);
  return 0;
}

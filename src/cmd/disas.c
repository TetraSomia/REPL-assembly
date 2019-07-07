#include <stdio.h>
#include <string.h>
#include "repl.h"
#include "getters.h"

static const char *_is_jump_to_label(const s_code_instruction *insts,
				     const s_code_instruction *inst) {
  char *label;

  if (inst->str_gen[0] != 'j')
    return NULL;
  for (label = inst->str_input + strlen(inst->str_input) - 1;
       label > inst->str_input && *label != ' ' && *label != '\t'; --label);
  if (label == inst->str_input)
    return NULL;
  label++;
  for (; insts; insts = insts->next)
    if (insts->label && strcmp(insts->label, label) == 0)
      return label;
  return NULL;
}

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
      printf("%d:\t   %s:\n\t%s %p\t%s", i->index, i->label, prefix,
	     i->address, i->str_gen);
    else
      printf("%d:\t%s %p\t%s", i->index, prefix, i->address, i->str_gen);
    const char *label = _is_jump_to_label(unit->insts, i);
    if (label)
      printf(" (%s)\n", label);
    else
      printf("\n");
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

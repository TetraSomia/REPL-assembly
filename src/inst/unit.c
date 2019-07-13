#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "repl.h"

s_code_unit *unit_find_from_addr(const void *addr) {
  for (int i = 0; context.units[i]; ++i)
    if ((void*)context.units[i]->code <= addr &&
	addr < (void*)(context.units[i]->code + context.units[i]->code_size))
      return context.units[i];
  return NULL;
}

s_code_unit *unit_find_from_name(const char *name) {
  for (int i = 0; context.units[i]; ++i)
    if (strcmp(context.units[i]->name, name) == 0)
      return context.units[i];
  return NULL;
}

void update_code_unit(s_code_unit *unit) {
  if (!unit || unit == context.cur_unit)
    return;
  printf("Switching current code unit: '%s' -> '%s'\n",
	 context.cur_unit->name, unit->name);
  context.cur_unit = unit;
}

s_code_unit *add_unit(char *name) {
  s_code_unit *new;
  int pagesize = getpagesize();
  int nbr_units;

  if (str_is_register_name(name) || !str_is_symbol_name(name)) {
    p_error("Bad code unit name: valid symbol name and not register name\n");
    return NULL;
  }
  for (nbr_units = 0; context.units && context.units[nbr_units]; ++nbr_units);
  context.units = xrealloc(context.units,
                           sizeof(s_code_unit*) * (nbr_units + 2));
  new = xmalloc(sizeof(s_code_unit));
  context.units[nbr_units] = new;
  context.units[nbr_units + 1] = NULL;

  new->name = xstrdup(name);
  new->code = mmap(NULL, pagesize, PROT_READ | PROT_WRITE | PROT_EXEC,
                   MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
  new->code_size = 0;
  new->code_max_size = pagesize;
  new->insts = NULL;

  add_instruction(new, NULL, "ret");

  return new;
}

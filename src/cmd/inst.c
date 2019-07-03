#include <stdio.h>
#include <string.h>
#include "repl.h"
#include "getters.h"

static int _inst_add(s_code_instruction *inst) {
  // Only sub command that can receive inst == NULL,
  // when we want to add a new instruction at the end
  (void)inst;
  return 0;
}

static int _inst_rm(s_code_instruction *inst) {
  (void)inst;
  return 0;
}

static int _inst_edit(s_code_instruction *inst) {
  (void)inst;
  return 0;
}

static struct {
  const char *name;
  int (*func)(s_code_instruction*);
} _sub_cmds[] =
  {{"add", &_inst_add},
   {"rm", &_inst_rm},
   {"edit", &_inst_edit},
   {NULL, NULL}};

int cmd_inst(int ac, char **av) {
  e_parsed_type inst_t = PARSED_ADDR;
  u_parsed_val inst_v;
  s_code_instruction *inst;

  if (ac < 1)
    return p_error("Please specify a sub command (add, rm, edit)\n");
  if (ac < 2) {
    if (!is_running())
      return p_error("Please specify a line index when code isn't running\n");
    inst_v.addr = get_reg(REG_RIP);
  } else
    inst_t = get_addr_or_idx(av[1], &inst_v);
  switch (inst_t) {
  case PARSED_ADDR:
    if ((inst = inst_find_from_addr((void*)inst_v.addr)) == NULL)
      return p_error("No instruction matches address 0x%lx\n", inst_v.addr);
    break;
  case PARSED_IDX:
    if ((inst = inst_find_from_idx(inst_v.idx)) == NULL &&
	!(inst_v.idx == -1 && strcmp(av[0], "add") == 0))
      return p_error("No instruction matches index %d in the current unit\n",
		     inst_v.idx);
    break;
  case PARSED_ERROR:
    return p_error("Third parameter not parsed (must be index or address)\n");
  }
  for (int i = 0; _sub_cmds[i].name; ++i)
    if (strcmp(_sub_cmds[i].name, av[0]) == 0)
      return _sub_cmds[i].func(inst);
  return p_error("\'%s\' doesn't match any sub command (add/rm/edit)\n", av[0]);
}

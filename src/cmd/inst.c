#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <stdlib.h>
#include "repl.h"
#include "getters.h"

static void _adjust_RIP(void *addr, size_t old_size, size_t new_size) {
  uint64_t *rip = get_reg_ptr(REG_RIP);

  if (!rip)
    return;
  if ((void*)*rip > addr)
    *rip += new_size - old_size;
}

static int _inst_add(s_code_instruction *inst) {
  void *old_addr = inst ? inst->address : NULL;
  char *line;
  s_code_instruction *new_inst;

  puts("Type the instruction to insert:");
  line = readline("> ");
  if (line == NULL)
    return p_warning("Aborting instruction insertion\n");
  new_inst = add_instruction(context.cur_unit, inst, line);
  free(line);
  if (new_inst)
    _adjust_RIP(old_addr, 0, new_inst->size);
  return new_inst ? 0 : 1;
}

static int _inst_rm(s_code_instruction *inst) {
  _adjust_RIP(inst->address, inst->size, 0);
  rm_instruction(context.cur_unit, inst);
  return 0;
}

static int _inst_edit(s_code_instruction *inst) {
  size_t old_size = inst->size;
  char *line;
  int err;

  printf("Editing the instruction \'%s\':\n", inst->str_gen);
  line = readline("> ");
  if (line == NULL)
    return p_warning("Aborting instruction edition\n");
  err = edit_instruction(context.cur_unit, inst, line);
  free(line);
  if (!err)
    _adjust_RIP(inst->address, old_size, inst->size);
  return err;
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
      return p_error("No instruction matches address 0x%lx"
		     " in the current unit\n", inst_v.addr);
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

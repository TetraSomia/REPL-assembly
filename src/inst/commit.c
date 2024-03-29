#include <stdlib.h>
#include <string.h>
#include "inst_internal.h"

static void _update_insts(s_code_unit *unit,
			  s_code_instruction *inst,
			  const s_parsed_inst *parsed_insts) {
  int i;

  for (i = 0; inst && parsed_insts[i].inst; ++i) {
    inst->size = parsed_insts[i + 1].inst ?
      parsed_insts[i + 1].offset - parsed_insts[i].offset :
      (int) unit->code_size - parsed_insts[i].offset;
    inst->address = unit->code + parsed_insts[i].offset;
    memcpy(inst->opcodes, inst->address, inst->size);
    if (inst->str_gen)
      free (inst->str_gen);
    inst->str_gen = parsed_insts[i].inst;
    inst->index = i;
    inst = inst->next;
  }
  if (inst || parsed_insts[i].inst)
    fatal_err("Assert failed: number of insts and"
	      "parsed_insts does not match\n");
}

static void _reset_breakpoints(s_code_unit *unit) {
  for (s_code_instruction *inst = unit->insts; inst; inst = inst->next)
    if (inst->breakpoint)
      *(inst->address) = INT3_OPCODE;
}

int commit_code(s_code_unit *unit) {
  s_parsed_inst *parsed_insts;

  if (parse_symbols(unit) != 0)
    return 1;
  parsed_insts = assemble(unit);
  if (parsed_insts == NULL)
    return 1;
  _update_insts(unit, unit->insts, parsed_insts);
  free(parsed_insts);
  _reset_breakpoints(unit);
  return 0;
}

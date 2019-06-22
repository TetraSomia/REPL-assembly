#include "code_unit.h"

void set_breakpoint(s_code_instruction *inst) {
  inst->breakpoint = true;
  *(inst->address) = INT3_OPCODE;
}

void reset_breakpoint(s_code_instruction *inst) {
  inst->breakpoint = false;
  *(inst->address) = inst->opcodes[0];
}

#include <stdio.h>
#include "code_unit.h"

void dump_code_instruction(s_code_instruction *inst) {
  puts("Dumping Code Instruction:");
  (void)inst;
  //TODO
}

void dump_code_unit(s_code_unit *unit) {
  puts("---------------------");
  puts("Dumping Code Unit:");
  printf("Code (size=%ld, size_max=%ld):\n",
	 unit->code_size, unit->code_max_size);
  for (s_code_instruction *inst = unit->insts; inst; inst = inst->next)
    dump_code_instruction(inst);
  puts("---------------------");
}

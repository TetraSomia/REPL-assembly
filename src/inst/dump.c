#include <stdio.h>
#include "code_unit.h"

void dump_code_instruction(s_code_instruction *inst) {
  puts("Dumping Code Instruction:");
  printf("opcodes: ");
  for (size_t i = 0; i < inst->size ; ++i)
    printf("%x ", inst->opcodes[i]);
  puts("");
  printf("size: %ld\n", inst->size);
  printf("str_gen: %s\n", inst->str_gen);
  printf("str_input: %s\n", inst->str_input);
  printf("label: %s\n", inst->label);
  printf("address: %p\n", inst->address);
  printf("index: %d\n", inst->index);
  printf("breakpoint: %s\n", inst->breakpoint ? "true" : "false");
}

void dump_code_unit(s_code_unit *unit) {
  puts("---------------------");
  puts("Dumping Code Unit:");
  printf("Code (size=%ld, size_max=%ld):\n",
	 unit->code_size, unit->code_max_size);
  //TODO print hex content
  for (s_code_instruction *inst = unit->insts; inst; inst = inst->next) {
    puts("");
    dump_code_instruction(inst);
  }
  puts("---------------------");
}

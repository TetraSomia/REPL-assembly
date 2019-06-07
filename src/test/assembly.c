#include <stdio.h>
#include <stdlib.h>
#include "assembly.h"

static void _test_assemble(const char *inst) {
  uint8_t *bytecode;
  size_t bytecode_size;
  int ret;

  ret = assemble(inst, &bytecode, &bytecode_size);
  if (ret != 0)
    fprintf(stderr, "assemble() failed to run command:\n%s", bytecode);
  else {
    for (size_t i = 0; i < bytecode_size; ++i)
      printf("0x%02x ", bytecode[i]);
    puts("");
  }
  free(bytecode);
}

static void _test_disassemble(const uint8_t *bytecode, size_t bytecode_size) {
  char *inst;
  disassemble(bytecode, bytecode_size, &inst);
  printf("%s", inst);
  free(inst);
}

void test_assembly()
{
  puts("Testing assemble() success:");
  _test_assemble("xor rax, rax");
  _test_assemble("syscall");
  _test_assemble("ret");
  _test_assemble("xor rax, rax\nsyscall\nret\n");
  puts("Testing assemble() failure:");
  _test_assemble("xor rax, rx");

  puts("");

  uint8_t bytecode[] = {0x48, 0x31, 0xc0, 0x0f, 0x05, 0xc3};
  puts("Testing disassemble():");
  _test_disassemble(bytecode, sizeof(bytecode));
}

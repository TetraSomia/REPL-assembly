#include <string.h>
#include <stdio.h>
#include "code_unit.h"

void parse_nasm_error(s_code_unit *unit, const char *err) {
  char msg[strlen(err)];
  const int header_size = 3;
  int line;
  int index = 0;

  if (sscanf(err, ".nasm_in:%d: %[^\n]", &line, msg) < 2)
    fatal_err("Assert failed: unrecognized NASM error format\n");
  if (msg[strlen(msg) - 1] == ']') {
    char *end = strrchr(msg, '[');
    if (end)
      *end = '\0';
  }
  line -= header_size;
  for (s_code_instruction *i = unit->insts; i; i = i->next) {
    if (i->label) {
      if (line == index) {
	fprintf(stderr, "%s (%d: \'%s\')\n", msg, i->index, i->str_sym);
	break;
      }
      index++;
    }
    if (line == index) {
      fprintf(stderr, "%s in \'%s\'\n", msg, i->str_sym);
      break;
    }
    index++;
  }
}

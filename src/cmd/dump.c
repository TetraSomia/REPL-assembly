#include "inst_internal.h"
#include "repl.h"

int cmd_dump(int ac, char* const *av) {
  (void)ac;
  (void)av;
  dump_code_unit(context.cur_unit);
  return 0;
}

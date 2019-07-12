#include "repl.h"

s_code_unit *unit_find_from_addr(const void *addr) {
  for (s_code_unit *u = &context.units[0]; u->name; ++u)
    if ((void*)u->code <= addr && addr < (void*)(u->code + u->code_size))
      return u;
  return NULL;
}

s_code_unit *unit_find_from_name(const char *name) {
  for (s_code_unit *u = &context.units[0]; u->name; ++u)
    if (u->name == name)
      return u;
  return NULL;
}

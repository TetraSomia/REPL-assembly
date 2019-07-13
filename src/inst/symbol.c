#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "repl.h"

#include <stdio.h>

typedef struct {
  const char *ref;
  char *dup;
  size_t len;
  void *addr;
} s_symbol;

static s_symbol *_extract_symbols(const char *inst) {
  s_symbol *syms = NULL;
  size_t nbr_syms = 0;

  for (const char *c = str_get_operand_start(inst); *c; ++c) {
    if (!isalpha(*c) && *c != '_')
      continue;
    const char *end = c;
    while (isalnum(*end) || *end == '_')
      end++;
    nbr_syms++;
    syms = xrealloc(syms, sizeof(s_symbol) * (nbr_syms + 1));
    syms[nbr_syms].dup = NULL;
    syms[nbr_syms].ref = NULL;
    syms[nbr_syms].len = 0;
    syms[nbr_syms - 1].ref = c;
    syms[nbr_syms - 1].len = end - c;
    syms[nbr_syms - 1].dup = xmalloc(syms[nbr_syms - 1].len + 1);
    memcpy(syms[nbr_syms - 1].dup, c, syms[nbr_syms - 1].len);
    syms[nbr_syms - 1].dup[syms[nbr_syms - 1].len] = '\0';
    c += syms[nbr_syms - 1].len;
  }
  return syms;
}

static void *_parse_code_units(const char *sym) {
  s_code_unit *unit = unit_find_from_name(sym);

  if (!unit)
    return NULL;
  return unit->code;
}

int parse_symbols(s_code_unit *unit) {
  for (s_code_instruction *i = unit->insts; i; i = i->next) {
    if (i->str_sym)
      continue;
    s_symbol *syms = _extract_symbols(i->str_input);
    for (int i = 0; syms && syms[i].len; ++i) {
      syms[i].addr = _parse_code_units(syms[i].dup);
      if (syms[i].addr)
	printf("Symbol found: replacing %s by %p\n", syms[i].dup, syms[i].addr);
    }
    i->str_sym = xstrdup(i->str_input);
  }
  return 0;
}

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "repl.h"

#include <stdio.h>

typedef struct {
  const char *ref;
  char *dup;
  size_t len;
  bool found;
  void *addr;
} s_symbol;

static s_symbol *_extract_symbols(const char *inst) {
  s_symbol *syms = NULL;
  size_t nbr_syms = 0;

  for (const char *c = str_get_operand_start(inst); *c; ++c) {
    if (*c != '$' || *(c + 1) == '\0')
      continue;
    c++;
    if (!isalpha(*c) && *c != '_')
      continue;
    const char *end = c;
    while (isalnum(*end) || *end == '_')
      end++;
    nbr_syms++;
    syms = xrealloc(syms, sizeof(s_symbol) * (nbr_syms + 1));
    syms[nbr_syms].ref = NULL;
    syms[nbr_syms].dup = NULL;
    syms[nbr_syms].len = 0;
    syms[nbr_syms].found = false;
    syms[nbr_syms].addr = NULL;
    syms[nbr_syms - 1].ref = c - 1;
    syms[nbr_syms - 1].len = (end - c) + 1;
    syms[nbr_syms - 1].found = false;
    syms[nbr_syms - 1].addr = NULL;
    syms[nbr_syms - 1].dup = xmalloc(syms[nbr_syms - 1].len);
    memcpy(syms[nbr_syms - 1].dup, c, syms[nbr_syms - 1].len - 1);
    syms[nbr_syms - 1].dup[syms[nbr_syms - 1].len - 1] = '\0';
    c += syms[nbr_syms - 1].len - 2;
  }
  return syms;
}

static void _parse_code_units(s_symbol *sym) {
  s_code_unit *unit = unit_find_from_name(sym->dup);

  if (unit) {
    sym->addr = unit->code;
    sym->found = true;
  }
}

static void _gen_str_sym(s_code_instruction *i, s_symbol *syms, size_t nbr) {
  const size_t address_max_size = 16;
  size_t sym_idx = 0;
  char *str_in;
  char *str_sym;

  if (nbr == 0) {
    i->str_sym = xstrdup(i->str_input);
    return;
  }
  i->str_sym = xmalloc(strlen(i->str_input) + (nbr * address_max_size) + 1);
  str_in = i->str_input;
  str_sym = i->str_sym;

  while (*str_in) {
    while (sym_idx < nbr && !syms[sym_idx].found)
      sym_idx++;
    if (sym_idx == nbr || str_in != syms[sym_idx].ref)
      *(str_sym++) = *(str_in++);
    else {
      if (syms[sym_idx].addr)
	str_sym += sprintf(str_sym, "%p", syms[sym_idx].addr);
      else
	str_sym += sprintf(str_sym, "0x0");
      str_in += syms[sym_idx].len;
      sym_idx++;
    }
  }
  *str_sym = '\0';
}

int parse_symbols(s_code_unit *unit) {
  for (s_code_instruction *i = unit->insts; i; i = i->next) {
    if (i->str_sym)
      continue;
    s_symbol *syms = _extract_symbols(i->str_input);
    size_t nbr_syms;
    for (nbr_syms = 0; syms && syms[nbr_syms].len; ++nbr_syms) {
      _parse_code_units(&syms[nbr_syms]);
      //TODO parse extern functions and variables
    }
    _gen_str_sym(i, syms, nbr_syms);
    for (int i = 0; syms && syms[i].len; ++i)
      free(syms[i].dup);
    free(syms);
  }
  return 0;
}

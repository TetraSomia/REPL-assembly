#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "reg_idx.h"

char *str_clear_sep(char *s) {
  int i;

  for (i = 0; s[i] == ' ' || s[i] == '\t'; ++i);
  memmove(s, s + i, strlen(s + i) + 1);
  for (i = strlen(s) - 1; i >= 0 && (s[i] == ' ' || s[i] == '\t'); --i);
  s[i + 1] = '\0';
  return s;
}

bool str_is_empty(const char *s) {
  for (; *s; ++s)
    if (*s != ' ' && *s != '\t')
      return false;
  return true;
}

bool str_is_comment(const char *s) {
  if (*s == ';')
    return true;
  return false;
}

const char *str_get_operand_start(const char *inst) {
  while (*inst && *inst != ' ' && *inst != '\t')
    inst++;
  while	(*inst && (*inst == ' ' || *inst == '\t'))
    inst++;
  return inst;
}

bool str_is_register_name(const char *s) {
  for (e_register r = REG_FIRST; r < REG_LAST; ++r)
    if (strcasecmp(reg_names[r], s) == 0)
      return true;
  return false;
}

bool str_is_symbol_name(const char *s) {
  for (int i = 0; s[i]; ++i) {
    if (i == 0) {
      if (!isalpha(s[i]) && s[i] != '_')
	return false;
    } else if (!isalnum(s[i]) && s[i] != '_')
      return false;
  }
  return true;
}

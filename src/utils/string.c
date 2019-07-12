#include <string.h>
#include "utils.h"

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

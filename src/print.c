#include <string.h>
#include <stdbool.h>
#include "print.h"

e_print_fmt print_parse_format(const char *s) {
  if (strlen(s) > 1)
    return PRINT_FMT_ERR;
  switch (*s) {
  case 'x':
    return PRINT_FMT_HEX;
  case 'd':
    return PRINT_FMT_DEC;
  case 's':
    return PRINT_FMT_STR;
  default:
    return PRINT_FMT_ERR;
  }
}

char print_parse_wordsize(const char *s) {
  if (strlen(s) > 1)
    return 0;
  unsigned char ws = *s - '0';
  bool ispowerof2 = ws && !(ws & (ws - 1));
  if (ispowerof2 && ws <= 8)
    return ws;
  return 0;
}

void print_mem(uint64_t *data, e_print_fmt fmt, char wordsize, size_t iter) {
  (void)data;
  (void)fmt;
  (void)wordsize;
  for (size_t i = 0; i < iter; ++i);
}

#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
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

static void _print_s(const char *s) {
  const char* const nonprint_names[] = {"\\a",
					"\\b",
					"\\t",
					"\\n",
					"\\v",
					"\\f",
					"\\r"};

  printf("%p:\t", s);
  while (*s) {
    if (isprint(*s))
      printf("%c", *s);
    else if ('\a' <= *s && *s <= '\r')
      printf("%s", nonprint_names[*s - '\a']);
    else
      break;
    s++;
  }
  printf("\n");
}

static void _dump_int(uint64_t *addr, char wordsize, bool hex) {
  switch (wordsize) {
  case 1:
    printf(hex ? "0x%02x" : "%d", (uint8_t)*addr);
    break;
  case 2:
    printf(hex ? "0x%04x" : "%d", (uint16_t)*addr);
    break;
  case 4:
    printf(hex ? "0x%08x" : "%d", (uint32_t)*addr);
    break;
  case 8:
    printf(hex ? "0x%016lx" : "%ld", (uint64_t)*addr);
    break;
  }
}

static void _print_int(uint64_t *addr, char wordsize, size_t iter, bool hex) {
  (void)addr;
  (void)wordsize;
  (void)iter;
  (void)hex;
}

int print_addr(uint64_t *addr, e_print_fmt fmt, char wordsize, size_t iter) {
  switch (fmt) {
  case PRINT_FMT_HEX:
    _print_int(addr, wordsize, iter, true);
    break;
  case PRINT_FMT_DEC:
    _print_int(addr, wordsize, iter, false);
    break;
  case PRINT_FMT_STR:
    _print_s((char*)addr);
    break;
  default:
    return 1;
  }
  return 0;
}

int print_reg(e_register reg, e_print_fmt fmt, char wordsize, size_t iter) {
  uint8_t *addr;
  
  printf("%s:\t", reg_names[reg]);
  addr = (uint8_t*)get_reg_ptr(reg);
  for (size_t i = 0; i < iter; ++i) {
    _dump_int((uint64_t*)addr, wordsize, fmt == PRINT_FMT_HEX);
    printf(" ");
    addr += wordsize;
  }
  printf("\n");
  return 0;
}

#pragma once

#include <stdint.h>
#include "getters.h"

typedef enum {
	      PRINT_FMT_ERR,
	      PRINT_FMT_HEX,
	      PRINT_FMT_DEC,
	      PRINT_FMT_STR
} e_print_fmt;

e_print_fmt print_parse_format(const char *s);
char print_parse_wordsize(const char *s);
int print_addr(uint64_t *addr, e_print_fmt fmt, char wordsize, size_t iter);
int print_reg(e_register reg, e_print_fmt fmt, char wordsize, size_t iter);

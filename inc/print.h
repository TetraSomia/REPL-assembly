#pragma once

#include <stdint.h>

typedef enum {
	      PRINT_FMT_ERR,
	      PRINT_FMT_HEX,
	      PRINT_FMT_DEC,
	      PRINT_FMT_STR
} e_print_fmt;

e_print_fmt print_parse_format(const char *s);
char print_parse_wordsize(const char *s);
void print_mem(uint64_t *data, e_print_fmt fmt, char wordsize, size_t iter);

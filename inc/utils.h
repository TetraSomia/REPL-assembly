#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
** Error and Warning report
*/

void fatal_err(const char *fmt, ...);
void fatal_libc_err(const char *fmt, ...); // same but uses errno
int p_warning(const char *fmt, ...);
int p_error(const char *fmt, ...);


/*
** Abort on failed allocation
*/

void *xrealloc(void *ptr, size_t size);
void *xmalloc(size_t size);
char *xstrdup(const char *s);

/*
** Assertions
*/

void assert_single_line(const char *s);

/*
** String manipulations and checks
*/

char *str_clear_sep(char *s);
bool str_is_empty(const char *s);
bool str_is_comment(const char *s);
const char *str_get_operand_start(const char *inst);
bool str_is_register_name(const char *s);
bool str_is_symbol_name(const char *s);

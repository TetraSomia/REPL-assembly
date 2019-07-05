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
** Term utils
*/

void clear_term();

/*
** Assertions
*/

void assert_single_line(const char *s);

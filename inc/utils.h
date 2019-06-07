#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
** Abort on error
*/

void fatal_err(const char *fmt, ...);
void fatal_libc_err(const char *fmt, ...); // same but uses errno


/*
** Abort on failed allocation
*/

void *xrealloc(void *ptr, size_t size);
void *xmalloc(size_t size);

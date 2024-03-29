#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "utils.h"

void fatal_err(const char *fmt, ...) {
  va_list arglist;

  va_start(arglist, fmt);
  vfprintf(stderr, fmt, arglist);
  va_end(arglist);
  exit(1);
}

void fatal_libc_err(const char *fmt, ...) {
  va_list arglist;
  int error = errno;

  va_start(arglist, fmt);
  vfprintf(stderr, fmt, arglist);
  va_end(arglist);
  fprintf(stderr, "Cause: %s\n", strerror(error));
  exit(1);
}

int p_error(const char *fmt, ...) {
  va_list arglist;

  fprintf(stderr, "Error: ");
  va_start(arglist, fmt);
  vfprintf(stderr, fmt, arglist);
  va_end(arglist);
  return 1;
}

int p_warning(const char *fmt, ...) {
  va_list arglist;

  fprintf(stderr, "Warning: ");
  va_start(arglist, fmt);
  vfprintf(stderr, fmt, arglist);
  va_end(arglist);
  return 0;
}

void *xmalloc(size_t size) {
  void *ptr;

  ptr = malloc(size);
  if (ptr == NULL)
    fatal_libc_err("malloc() failed\n");
  return ptr;
}

void *xrealloc(void *ptr, size_t size) {
  ptr = realloc(ptr, size);
  if (ptr == NULL)
    fatal_libc_err("realloc() failed\n");
  return ptr;
}

char *xstrdup(const char *s) {
  char *dup;

  dup = strdup(s);
  if (dup == NULL)
    fatal_libc_err("strdup() failed\n");
  return dup;
}

inline void assert_single_line(const char *s) {
  if (strchr(s, '\n'))
    fatal_err("Assert failed: multi-line string used\n");
}

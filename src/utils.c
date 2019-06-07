#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "utils.h"

void fatal_err(const char *fmt, ...)
{
  va_list arglist;

  va_start(arglist, fmt);
  vfprintf(stderr, fmt, arglist);
  va_end(arglist);
  exit(1);
}

void fatal_libc_err(const char *fmt, ...)
{
  va_list arglist;
  int error = errno;

  va_start(arglist, fmt);
  vfprintf(stderr, fmt, arglist);
  va_end(arglist);
  fprintf(stderr, "Cause: %s\n", strerror(error));
  exit(1);
}


void *xmalloc(size_t size)
{
  void *ptr;

  ptr = malloc(size);
  if (ptr == NULL)
    fatal_libc_err("malloc() failed");
  return ptr;
}

void *xrealloc(void *ptr, size_t size)
{
  ptr = realloc(ptr, size);
  if (ptr == NULL)
    fatal_libc_err("realloc() failed");
  return ptr;
}

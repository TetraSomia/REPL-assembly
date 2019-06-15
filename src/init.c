#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "repl.h"

s_context context;

static int _pagesize;

static void _free_context() {
  munmap(context.units[0].code, _pagesize);
  free(context.units[0].name);
  free(context.units);
}

void init_context() {
  context.exec_ctx = NULL;
  context.units = xmalloc(sizeof(s_code_unit));

  context.units[0].name = strdup("main");
  _pagesize = getpagesize();
  context.units[0].code = mmap(NULL, _pagesize,
			       PROT_READ | PROT_WRITE | PROT_EXEC,
			       MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
  context.units[0].code_size = 0;
  context.units[0].insts = NULL;
  atexit(&_free_context);
}

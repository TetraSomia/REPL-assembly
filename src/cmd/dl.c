#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include "repl.h"

static bool _dynsym_already_loaded(const char *sym) {
  if (!context.dyn_syms)
    return false;
  for (int i = 0; context.dyn_syms[i].sym; ++i)
    if (strcmp(context.dyn_syms[i].sym, sym) == 0)
      return true;
  return false;
}

static int _load_dynsym(const char *lib, const char *sym) {
  void *handler;
  void *sym_addr;
  int n_sym;

  if (_dynsym_already_loaded(sym))
    return p_warning("Symbol already loaded.\n");
  handler = dlopen(lib, RTLD_LAZY | RTLD_NODELETE);
  if (handler == NULL)
    return p_error("Library '%s' not found.\n", lib);
  sym_addr = dlsym(handler, sym);
  if (sym_addr == NULL) {
    dlclose(handler);
    return p_error("Symbol '%s' not found.\n", sym);
  }
  for (n_sym = 0; context.dyn_syms && context.dyn_syms[n_sym].sym; ++n_sym);
  context.dyn_syms = xrealloc(context.dyn_syms, sizeof(s_dyn_sym) * (n_sym+2));
  context.dyn_syms[n_sym + 1].lib = NULL;
  context.dyn_syms[n_sym + 1].sym = NULL;
  context.dyn_syms[n_sym + 1].addr = NULL;
  context.dyn_syms[n_sym].lib = xstrdup(lib);
  context.dyn_syms[n_sym].sym = xstrdup(sym);
  context.dyn_syms[n_sym].addr = sym_addr;
  dlclose(handler);
  return 0;
}

static int _print_dynsyms() {
  if (!context.dyn_syms)
    return p_warning("No dynamically loaded symbols yet.\n");
  puts("Listing dynamically loaded symbols:");
  for (int i = 0; context.dyn_syms[i].sym; ++i)
    printf("%s: %s: %p\n", context.dyn_syms[i].lib,
	   context.dyn_syms[i].sym, context.dyn_syms[i].addr);
  return 0;
}

int cmd_dl(int ac, char* const *av) {
  if (ac == 0)
    return _print_dynsyms();
  if (ac != 2)
    return p_error("Invalid number of arguments, must be 0 or 2.\n");
  return _load_dynsym(av[0], av[1]);
}

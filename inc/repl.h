#pragma once

#include <ucontext.h>
#include "utils.h"
#include "code_unit.h"

#define DEFAULT_STACK_SIZE 32768

typedef struct {
  char *lib;
  char *sym;
  void *addr;
} s_dyn_sym;

typedef struct {
  char *name;
  uint8_t *array;
  size_t size;
  bool is_str;
} s_array;

typedef struct {
  void *sighandler_stack;
  void *stack;
  size_t stack_size;
  ucontext_t *exec_ctx;
  s_code_unit **units;
  s_code_unit *cur_unit;
  s_dyn_sym *dyn_syms;
  s_array *arrays;
} s_context;

extern s_context context;

void init_context();
void repl();

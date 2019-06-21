#pragma once

#include <ucontext.h>
#include "utils.h"
#include "code_unit.h"

#define DEFAULT_STACK_SIZE 16384

typedef struct {
  void *stack;
  size_t stack_size;
  ucontext_t *exec_ctx;
  s_code_unit *units;
} s_context;

extern s_context context;

void init_context();
void repl();

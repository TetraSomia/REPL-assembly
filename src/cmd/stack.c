#include <stdio.h>
#include <string.h>
#include "repl.h"
#include "getters.h"

typedef enum {
	      STACKMODE_ALL = 0,
	      STACKMODE_FRAME,
	      STACKMODE_OFFSET,
	      STACKMODE_ERROR
} e_stackmode;

static e_stackmode _parse_stackmode(const char *s, size_t *offset) {
  if (strcmp(s, "all") == 0)
    return STACKMODE_ALL;
  if (strcmp(s, "frame") == 0)
    return STACKMODE_FRAME;
  if (sscanf(s, "%ld", offset) == 1)
    return STACKMODE_OFFSET;
  return STACKMODE_ERROR;
}

static void _print_header(void *rsp, void *rbp, void *p) {
  if (p == rsp)
    printf("SP ");
  else if (p == rbp)
    printf("BP ");
  else
    printf("   ");
}

static bool _is_valid_stack_addr(void *p) {
  if (p < context.stack)
    return false;
  if (p > (context.stack + context.stack_size - 8))
    return false;
  return true;
}

static bool _is_stack_empty(uint64_t *p) {
  while (_is_valid_stack_addr(p)) {
    if (*p != 0)
      return false;
    p--;
  }
  return true;
}

static inline void *_get_top_addr() {
  return context.stack + context.stack_size - 8;
}

static int _p_all(void *rsp, void *rbp, size_t ignore) {
  (void)ignore;
  for (uint64_t *p = _get_top_addr();_is_valid_stack_addr(p); --p) {
    _print_header(rsp, rbp, p);
    printf("%p:\t0x%016lx\n", p, *p);
    if (_is_stack_empty(p))
      break;
  }
  return 0;
}

static int _p_frame(void *rsp, void *rbp, size_t ignore) {
  (void)ignore;
  if (!is_running())
    return p_error("Code isn't running\n");
  if (!_is_valid_stack_addr(rsp) || !_is_valid_stack_addr(rbp))
    return p_error("RSP or RBP do not point into the stack\n");
  if (rbp < rsp)
    return p_error("RBP < RSP\n");
  for (uint64_t *p = rbp; _is_valid_stack_addr(p) && (void*)p >= rsp; --p) {
    _print_header(rsp, rbp, p);
    printf("%p:\t0x%016lx\n", p, *p);
  }
  return 0;
}

static int _p_offset(void *rsp, void *rbp, size_t offset) {
  uint64_t *p = rsp;
  uint64_t *end = rsp;

  if (!is_running())
    return p_error("Code isn't running\n");
  if (!_is_valid_stack_addr(rsp))
    return p_error("RSP does not point to a valid stack address\n");
  p += offset;
  end -= offset;
  while (!_is_valid_stack_addr(p))
    p--;
  while (_is_valid_stack_addr(p) && p >= end) {
    _print_header(rsp, rbp, p);
    printf("%p:\t0x%016lx\n", p, *p);
    if (_is_stack_empty(p))
      break;
    p--;
  }
  return 0;
}

int cmd_stack(int ac, char* const *av) {
  size_t offset = 5;
  e_stackmode mode = STACKMODE_OFFSET;
  int (*p_func[])(void*, void*, size_t) = {&_p_all, &_p_frame, &_p_offset};

  if (ac > 0)
    if ((mode = _parse_stackmode(av[0], &offset)) == STACKMODE_ERROR)
      return p_error("\'%s\' cannot be parsed into a valid mode "
		     "(all | frame | <+-offset>)\n", av[0]);
  return p_func[mode]((void*)get_reg(REG_RSP), (void*)get_reg(REG_RBP), offset);
}

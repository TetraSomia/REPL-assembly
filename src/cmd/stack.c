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

static void _p_all(void *rsp, size_t ignore) {
  (void)ignore;
  (void)rsp;
  puts("all");
}

static void _p_frame(void *rsp, size_t ignore) {
  (void)ignore;
  (void)rsp;
  puts("frame");
}

static void _p_offset(void *rsp, size_t offset) {
  (void)rsp;
  printf("offset: %ld\n", offset);
}

int cmd_stack(int ac, char* const *av) {
  size_t offset = 5;
  e_stackmode mode = STACKMODE_OFFSET;
  void (*p_func[])(void*, size_t) = {&_p_all, &_p_frame, &_p_offset};

  if (ac > 0)
    if ((mode = _parse_stackmode(av[0], &offset)) == STACKMODE_ERROR)
      return p_error("\'%s\' cannot be parsed into a valid mode "
		     "(all | frame | <+-offset>)\n", av[0]);
  p_func[mode]((void*)get_reg(REG_RSP), offset);
  return 0;
}

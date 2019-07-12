#include <stdio.h>
#include <string.h>
#include "repl.h"

typedef enum {
	      STACKMODE_ALL = 0,
	      STACKMODE_FRAME,
	      STACKMODE_OFFSET,
	      STACKMODE_ERROR
} e_stackmode;

e_stackmode _parse_stackmode(const char *s, size_t *offset) {
  if (strcmp(s, "all") == 0)
    return STACKMODE_ALL;
  if (strcmp(s, "frame") == 0)
    return STACKMODE_FRAME;
  if (sscanf(s, "%ld", offset) == 1)
    return STACKMODE_OFFSET;
  return STACKMODE_ERROR;
}

static void _p_all() {
  puts("all");
}

static void _p_frame() {
  puts("frame");
}

static void _p_offset(size_t offset) {
  printf("offset: %ld\n", offset);
}

int cmd_stack(int ac, char* const *av) {
  size_t offset = 5;
  e_stackmode mode = STACKMODE_OFFSET;
  void (*p_func[])(size_t) = {&_p_all, &_p_frame, &_p_offset};

  if (ac > 0)
    if ((mode = _parse_stackmode(av[0], &offset)) == STACKMODE_ERROR)
      return p_error("\'%s\' cannot be parsed into a valid mode "
		     "(all | frame | <+-size>)\n", av[0]);
  p_func[mode](offset);
  return 0;
}

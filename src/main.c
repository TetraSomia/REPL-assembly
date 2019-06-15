#include <stdio.h>
#include "test.h"
#include "repl.h"

static void _run_tests() {
  test_assembly();
}

int main(int ac, char **av)
{
  (void)av;
  if (ac > 1) {
    _run_tests();
    return 0;
  }
  init_context();
  repl();
  return 0;
}

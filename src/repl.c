#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "repl.h"

static int _getline(char **lineptr, size_t *n) {
  ssize_t getline_ret;

  printf("?> ");
  getline_ret = getline(lineptr, n, stdin);
  if (getline_ret == -1 && errno != 0)
    fatal_libc_err("getline() failed\n");
  if ((getline_ret == -1 && errno == 0) || strcmp(*lineptr, "exit\n") == 0)
    return 1;
  (*lineptr)[getline_ret - 1] = '\0';
  return 0;
}

void repl() {
  char *line = NULL;
  size_t line_size = 0;
  
  while (1) {
    if (_getline(&line, &line_size) != 0)
      break;
    printf("%s\n", line);
  }
  free(line);
  puts("exiting...");
}

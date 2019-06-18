#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "repl.h"
#include "commands.h"

static struct { const char *name; f_command func; } _cmds[] =
  {{"help", &cmd_help},
   {"dump", &cmd_dump},
   {NULL, NULL}};

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

static char **_tokenize_args(const char *line, size_t line_size, int *ac_ptr) {
  char tmp_line[line_size]; // no +1 because we replaced the '\n' with a '\0'
  char *tmp_tok;
  char **toks = NULL;
  size_t nbr_toks = 0;

  memcpy(tmp_line, line, line_size);
  tmp_tok = strtok(tmp_line, " ");
  while (tmp_tok) {
    nbr_toks ++;
    toks = xrealloc(toks, sizeof(*toks) * (nbr_toks + 1));
    toks[nbr_toks - 1] = xstrdup(tmp_tok);
    toks[nbr_toks] = NULL;
    tmp_tok = strtok(NULL, " ");
  }
  *ac_ptr = nbr_toks - 1;
  return toks;
}

static void _free_toks(char **toks) {
  for (int i = 0; toks[i]; ++i)
    free(toks[i]);
  free(toks);
}

void repl() {
  char *line = NULL;
  size_t line_size = 0;
  char **toks;
  int ac;
  int cmd_ret;
  
  while (1) {
    if (_getline(&line, &line_size) != 0)
      break;
    toks = _tokenize_args(line, line_size, &ac);
    if (!toks)
      continue;
    cmd_ret = -1;
    for (int i = 0; _cmds[i].name; ++i) {
      if (strcmp(_cmds[i].name, toks[0]) == 0)
	cmd_ret = _cmds[i].func(ac, toks + 1);
    }
    if (cmd_ret == -1)
      fprintf(stderr, "Command not found: %s\n", toks[0]);
    else if (cmd_ret != 0)
      fprintf(stderr, "Command failed: %s\n", toks[0]);
    _free_toks(toks);
  }
  free(line);
  puts("exiting...");
}

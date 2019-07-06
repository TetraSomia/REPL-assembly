#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <readline/readline.h>
#include "repl.h"
#include "commands.h"
#include "context_switch.h"

static struct {
  const char *name;
  const char *short_name;
  f_command func; } _cmds[] =
    {{"help", "h", &cmd_help},
     {"dump", "dump", &cmd_dump},
     {"run", "r", &cmd_run},
     {"continue", "c", &cmd_cont},
     {"disas", "d", &cmd_disas},
     {"inst", "i", &cmd_inst},
     {"label", "l", &cmd_label},
     {"breakpoint", "b", &cmd_breakpoint}};

static char **_tokenize_args(const char *line, size_t line_size, int *ac_ptr) {
  char tmp_line[line_size];
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

static void _free_toks(char **toks, char *line) {
  free(line);
  if (!toks)
    return;
  for (int i = 0; toks[i]; ++i)
    free(toks[i]);
  free(toks);
}

void repl() {
  char *line = NULL;
  char **toks = NULL;
  int ac;
  int cmd_ret;

  if (getcontext(ctx_get_repl_ctx()) != 0)
    fatal_libc_err("getcontext(&_repl_ctx) failed\n");
  ctx_handle_ctx_update();
  while (1) {
    _free_toks(toks, line);
    if ((line = readline("?> ")) == NULL || strcmp(line, "exit") == 0)
      break;
    toks = _tokenize_args(line, strlen(line) + 1, &ac);
    if (!toks)
      continue;
    cmd_ret = -1;
    for (size_t i = 0; i < sizeof(_cmds) / sizeof(*_cmds); ++i) {
      if (strcmp(_cmds[i].name, toks[0]) == 0 ||
	  strcmp(_cmds[i].short_name, toks[0]) == 0)
	cmd_ret = _cmds[i].func(ac, toks + 1);
    }
    if (cmd_ret == -1)
      fprintf(stderr, "Command not found: %s\n", toks[0]);
    // else if (cmd_ret != 0) //Command failed
  }
  free(line);
  puts("exiting...");
}

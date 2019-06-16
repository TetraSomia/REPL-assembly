#include <stdio.h>

static void _print_global_help() {
  puts("Here are the different available commands:\n");
  puts("help [cmd]\tPrint this message, give further information "
       "on the 'cmd' command if specified");
}

int cmd_help(int ac, char **av) {
  (void)av;
  if (ac == 0)
    _print_global_help();
  else
    puts("Not implemented yet");
  return 0;
}

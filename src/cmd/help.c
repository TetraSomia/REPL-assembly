#include <stdio.h>

static void _print_global_help() {
  puts("Here are the commands available:\n");
  puts("help [<cmd>]\n\tPrint this message, give further information"
       "on the 'cmd'\n\tcommand if specified.\n");
  puts("run\n\tStart and run the assembly code.\n");
  puts("continue\n\tResume execution after a breakpoint hit.\n");
  puts("disas\n\tDisassemble the code of the current unit.\n");
  puts("breakpoint (<index> | <addr>) [!]\n\tAdd or remove a breakpoint.\n");
  puts("label (<index> | <addr>) [!]\n\tAdd or remove a label.\n");
  puts("inst (add | rm | edit) [<index> | <addr>]\n\tAdd, remove, or edit a "
       "specific instruction, referenced through\n\tthe second parameter,"
       " or implicitely through RIP if the code is\n\trunning. Note: adding "
       "an instruction to index=1 will create an\n\tinstruction with index=2, "
       "then specifying index=-1 is allowed\n\tto emplace a"
       " first instruction.\n");
  puts("print (<reg> | <addr>) [(s | x | d) [(1 | 2 | 4 | 8) [<iter>]]]\n\t"
       "Print the content of a register or at a particular address.\n\t"
       "The format can be a string (s), a decimal value (d) or a\n\thexadecimal"
       " value (x), default is hexadecimal.\n\tThe word size can be specified "
       "(default is 8).\n\tFinally, it is possible to print <iter> words"
       " (starting\n\tat the address, or within the register).\n");
  puts("Aliases: first letter of the command");
}

int cmd_help(int ac, char* const *av) {
  (void)av;
  if (ac == 0)
    _print_global_help();
  else
    puts("Not written yet, you'll have to look into the code");
  return 0;
}

#include <stdio.h>

static void _print_global_help() {
  puts("Here are the commands available:\n");
  puts("help [<cmd>]\n\tPrint this message, give further information"
       "on the 'cmd'\n\tcommand if specified.\n");
  puts("run\n\tStart and run the assembly code.\n");
  puts("continue\n\tResume execution after a breakpoint hit.\n");
  puts("disas\n\tDisassemble the code of the current code unit.\n");
  puts("breakpoint (<index> | <addr>) [!]\n\tAdd or remove a breakpoint.\n");
  puts("label (<index> | <addr>) [!]\n\tAdd or remove a label.\n");
  puts("inst (add | rm | edit) [<index> | <addr>]\n\tAdd, remove, or edit a "
       "specific instruction, referenced through\n\tthe second parameter,"
       " or implicitely through RIP if the code is\n\trunning. Note: adding "
       "an instruction to index=1 will create an\n\tinstruction with index=2, "
       "then specifying index=-1 is allowed\n\tto emplace a"
       " first instruction.\n");
  puts("print [*]...(<reg> | <addr>) [(s | x | d) [(1 | 2 | 4 | 8) [<iter>]]]\n"
       "\tPrint the content of a register or at a particular address.\n\t"
       "The format can be a string (s), a decimal value (d) or a\n\thexadecimal"
       " value (x), default is hexadecimal.\n\tThe word size can be specified "
       "(default is 8).\n\tFinally, it is possible to print <iter> words"
       " (starting\n\tat the address, or within the register).\n\t"
       "Star operator (*) can be used to dereference once or multiple times\n\t"
       "the register or address.\n");
  puts("set [*]...(<reg> | <addr>) [1 | 2 | 4 | 8] <hex value>\n\t"
       "Set a register or the content of an address at a particular value.\n\t"
       "Star operator (*) can be used to dereference once or multiple times\n\t"
       "the register or address.\n");
  puts("stack [all | frame | <+-offset>]\n\t"
       "Print stack content depending on 3 different modes:\n\t"
       "all: print the whole allocated stack space (stop when reaching 0's)\n\t"
       "frame: if RBP is set correctly, print the section between RBP and RSP\n"
       "\toffset=x: print x lines before and after RSP (default with x=5)\n");
  puts("unit [<name>]\n\tWithout parameter, shows the units available and "
       "highlight the active\n\tone. If a code unit name is provided, sets this"
       " unit as active. If the\n\tunit does not exist, ask if we want to"
       " create it. A code unit can be\n\tseen as a new assembly source file or"
       " as a new function.\n");
  puts("Aliases: first letter of the command (except for \'set\', \'stack\' "
       "has priority)");
}

int cmd_help(int ac, char* const *av) {
  (void)av;
  (void)ac;
  _print_global_help();
  return 0;
}

#pragma once

typedef int (*f_command)(int ac, char* const *av);

int cmd_help(int ac, char* const *av);
int cmd_dump(int ac, char* const *av);
int cmd_run(int ac, char* const *av);
int cmd_cont(int ac, char* const *av);
int cmd_disas(int ac, char* const *av);
int cmd_inst(int ac, char* const *av);
int cmd_label(int ac, char* const *av);
int cmd_breakpoint(int ac, char* const *av);
int cmd_print(int ac, char* const *av);
int cmd_set(int ac, char* const *av);
int cmd_stack(int ac, char* const *av);

#pragma once

typedef int (*f_command)(int ac, char **av);

int cmd_help(int ac, char **av);
int cmd_dump(int ac, char **av);
int cmd_run(int ac, char **av);
int cmd_cont(int ac, char **av);
int cmd_disas(int ac, char **av);
int cmd_inst(int ac, char **av);

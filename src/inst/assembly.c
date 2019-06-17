//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
//#include <fcntl.h>
#include <errno.h>
#include "inst_internal.h"

static const char _path_file_in[] = ".nasm_in";
static const char _path_file_out[] = ".nasm_out";

static void _cleanup_tmpfile() {
  remove(_path_file_in);
  remove(_path_file_out);
  //todo register
}

static void _write_code(s_code_instruction *insts) {
  FILE *file;

  file = fopen(_path_file_in, "w");
  if (file == NULL)
    fatal_libc_err("fopen(\"%s\") failed\n", _path_file_in);
  if (fprintf(file, "[BITS 64]\n\n") < 0)
    fatal_err("fprintf() failed\n");
  for (s_code_instruction *inst = insts; inst; inst = inst->next) {
    if (inst->label) {
      if (fprintf(file, "%s:\n", inst->label) < 0)
	fatal_err("fprintf() failed\n");
    }
    if (fprintf(file, "\t%s\n", inst->str_input) < 0)
      fatal_err("fprintf() failed\n");
  }
  if (fclose(file) == EOF)
    fatal_libc_err("fclose(\"%s\") failed\n", _path_file_in);
}

static int _assemble() {
  const char cmd_fmt[] = "nasm -Werror -w+all -s -a -fbin -o %s %s";
  char cmd[sizeof(cmd_fmt) + sizeof(_path_file_in) + sizeof(_path_file_out)];
  FILE *file;
  char *line = NULL;
  size_t line_len = 0;
  ssize_t read;
  int is_error = 0;

  sprintf(cmd, cmd_fmt, _path_file_out, _path_file_in);
  file = popen(cmd, "r");
  if (file == NULL)
    fatal_libc_err("popen(\"%s\") failed\n", cmd);
  while ((read = getline(&line, &line_len, file)) != -1) {
    if (is_error == 0)
      fprintf(stderr, "An error occured when assembling:\n");
    is_error = 1;
    fprintf(stderr, "%s", line);
  }
  if (errno != 0)
    fatal_libc_err("getline() on popen() failed\n");
  if (line)
    free(line);
  if (pclose(file) == -1)
    fatal_libc_err("pclose(\"%s\") failed", cmd);
  return is_error;
}

static void _read_assembly() {
}

static void _disassemble() {
  const char cmd_fmt[] = "objdump -D --no-show-raw-insn "
    "-b binary -m i386:x86-64 -M intel %s";
  char cmd[sizeof(cmd_fmt) + sizeof(_path_file_out)];

  sprintf(cmd, cmd_fmt, _path_file_out);
}

s_parsed_inst *assemble(s_code_unit *unit) {
  s_parsed_inst *parsed_insts = NULL;

  _write_code(unit->insts);
  if (_assemble() != 0)
    return NULL;
  _read_assembly();
  _disassemble();
  atexit(&_cleanup_tmpfile);
  return parsed_insts;
}

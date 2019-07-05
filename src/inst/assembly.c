#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "inst_internal.h"

static const char _path_file_in[] = ".nasm_in";
static const char _path_file_out[] = ".nasm_out";

static void _cleanup_tmpfile() {
  remove(_path_file_in);
  remove(_path_file_out);
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
  const char cmd_fmt[] = "nasm -Xgnu -Werror -w+all -s -a -fbin -o %s %s";
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
  errno = 0;
  while ((read = getline(&line, &line_len, file)) != -1) {
    if (is_error == 0)
      fprintf(stderr, "An error occured when assembling:\n");
    is_error = 1;
    fprintf(stderr, "%s", line);
  }
  if (errno != 0)
    fatal_libc_err("getline() on popen() failed\n");
  free(line);
  if (pclose(file) == -1)
    fatal_libc_err("pclose(\"%s\") failed\n", cmd);
  return is_error;
}

static int _read_assembly(s_code_unit *unit) {
  char buff[512];
  FILE *file;
  size_t read_len;
  size_t idx = 0;
  int err = 0;

  file = fopen(_path_file_out, "r");
  if (file == NULL)
    fatal_libc_err("fopen(\"%s\") failed\n", _path_file_out);

  do {
    read_len = fread(buff, 1, sizeof(buff), file);
    if (read_len == 0)
      break;
    if (idx + read_len > unit->code_max_size) {
      //TODO: try to realloc at same address with mmap
      err = 1;
      p_error("Maximum number of bytes wrote into "
	      "the code_unit (%ld), code truncated at the end\n"
	      "Last byte of code at address: %p\n", unit->code_max_size,
	      unit->code + unit->code_max_size - 1);
      read_len = unit->code_max_size - idx;
    }
    memcpy(unit->code + idx, buff, read_len);
    idx += read_len;
  } while (read_len == sizeof(buff) && !err);
  unit->code_size = idx;
  if (ferror(file))
    fatal_libc_err("fread(\"%s\") failed\n", _path_file_out);  
  if (fclose(file) == EOF)
    fatal_libc_err("fclose(\"%s\") failed\n", _path_file_out);
  return err;
}

static s_parsed_inst *_parse_disassembled_line(s_parsed_inst *insts,
					       const char *line, int line_nbr) {
  insts = xrealloc(insts, sizeof(s_parsed_inst) * (line_nbr + 2));
  insts[line_nbr + 1].offset = -1;
  insts[line_nbr + 1].inst = NULL;
  insts[line_nbr].inst = xmalloc(strlen(line) + 1);
  sscanf(line, "%x: %[^\n]", &insts[line_nbr].offset, insts[line_nbr].inst);
  return insts;
}

static s_parsed_inst * _disassemble() {
  const char cmd_fmt[] = "objdump -D --no-show-raw-insn "
    "-b binary -m i386:x86-64 -M intel %s";
  char cmd[sizeof(cmd_fmt) + sizeof(_path_file_out)];
  FILE *file;
  char *line = NULL;
  size_t line_len = 0;
  ssize_t read;
  int line_nbr = -1;
  const int header_size = 7;
  s_parsed_inst *insts = NULL;

  sprintf(cmd, cmd_fmt, _path_file_out);
  file = popen(cmd, "r");
  if (file == NULL)
    fatal_libc_err("popen(\"%s\") failed\n", cmd);
  errno = 0;
  while ((read = getline(&line, &line_len, file)) != -1) {
    line_nbr++;
    if (line_nbr < header_size)
      continue;
    insts = _parse_disassembled_line(insts, line, line_nbr - header_size);
  }
  if (errno != 0)
    fatal_libc_err("getline() on popen() failed\n");
  free(line);
  if (pclose(file) == -1)
    fatal_libc_err("pclose(\"%s\") failed\n", cmd);
  if (insts == NULL)
    fatal_err("Assert failed: objdump didn't output any instruction\n");
  return insts;
}

s_parsed_inst *assemble(s_code_unit *unit) {
  static bool cleanup_registered = false;

  if (!cleanup_registered) {
    atexit(&_cleanup_tmpfile);
    cleanup_registered = true;
  }
  _write_code(unit->insts);
  if (_assemble() != 0)
    return NULL;
  _read_assembly(unit);
  return _disassemble();
}

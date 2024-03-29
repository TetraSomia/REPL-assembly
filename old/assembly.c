#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "assembly.h"

static const char _path_tmpfile[] = ".assemblytmp";
static int _fd_tmpfile = -1;

static void _cleanup_tmpfile(void) {
  close(_fd_tmpfile);
  remove(_path_tmpfile);
}

static void _init_tmpfile() {
  _fd_tmpfile = open(_path_tmpfile, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (_fd_tmpfile == -1)
    fatal_libc_err("open() failed on temporary file\n");
  atexit(&_cleanup_tmpfile);
}

static int _exec_popen(const char *cmd, uint8_t **data, size_t *size) {
  char buff[256];
  size_t len;
  FILE *read_pipe;

  read_pipe = popen(cmd, "r");
  if (read_pipe == NULL)
    fatal_err("popen() failed with this command: %s\n", cmd);
  *data = NULL;
  *size = 0;
  do {
    len = fread(buff, 1, sizeof(buff), read_pipe);
    if (len == 0)
      break;
    size_t new_size = *size + len;
    *data = xrealloc(*data, new_size + 1);
    memcpy(*data + *size, buff, len);
    *size = new_size;
  } while (len == sizeof(buff));
  int ret = pclose(read_pipe);
  if (ret == -1)
    fatal_libc_err("pclose() failed\n", cmd);
  (*data)[*size] = '\0'; // To allow interpretation of '*data' as a string
  return ret == 0 ? 0 : 1;
}

static void _write_to_file(bool need_reset, const uint8_t *data, size_t size) {
  if (need_reset)
    if (lseek(_fd_tmpfile, 0, SEEK_SET) == (off_t) -1)
      fatal_libc_err("lseek() failed on temporary file\n");
  write(_fd_tmpfile, data, size);
  if (need_reset)
    if (ftruncate(_fd_tmpfile, size) == -1)
      fatal_libc_err("ftruncate() failed on temporary file\n");
}

static void _remove_objdump_header(char *inst) {
  size_t nbr_line_to_remove = 7;
  size_t begin_idx = 0;

  for (size_t i = 0; i < nbr_line_to_remove; ++i) {
    while (inst[begin_idx] != '\0' && inst[begin_idx++] != '\n');
    if (inst[begin_idx] == '\0')
      fatal_err("ASSERT FAILED: objdump header seems to have changed size\n");
  }
  memmove(inst, inst + begin_idx, strlen(inst + begin_idx) + 1);
}

int assemble(const char *inst, uint8_t **bytecode, size_t *bytecode_size) {
  const char cmd_fmt[] = "nasm -Werror -w+all -s -a -fbin -o /dev/stdout %s";
  const char header[] = "[BITS 64]\n";
  char cmd[sizeof(cmd_fmt) + sizeof(_path_tmpfile)];
  char content[sizeof(header) + strlen(inst) + 1];
  bool need_reset = false;

  if (inst == NULL || strlen(inst) == 0 ||
      bytecode == NULL || bytecode_size == NULL)
    fatal_err("assemble() received bad argument\n");
  if (_fd_tmpfile == -1)
    _init_tmpfile();
  else
    need_reset = true;
  strcpy(content, header);
  strcat(content, inst);
  strcat(content, "\n");
  _write_to_file(need_reset, (uint8_t*)content, sizeof(content) - 1);
  sprintf(cmd, cmd_fmt, _path_tmpfile);
  return _exec_popen(cmd, bytecode, bytecode_size);
}

void disassemble(const uint8_t *bytecode, size_t bytecode_size, char **inst) {
  const char cmd_fmt[] = "objdump -D -b binary -m i386:x86-64 -M intel %s";
  char cmd[sizeof(cmd_fmt) + sizeof(_path_tmpfile)];
  bool need_reset = false;
  size_t output_size;

  if (bytecode == NULL || bytecode_size == 0 || inst == NULL)
    fatal_err("disassemble() received bad argument\n");
  if (_fd_tmpfile == -1)
    _init_tmpfile();
  else
    need_reset = true;
  _write_to_file(need_reset, bytecode, bytecode_size);
  sprintf(cmd, cmd_fmt, _path_tmpfile);
  if (_exec_popen(cmd, (uint8_t**)inst, &output_size) != 0)
    fatal_err("ASSERT FAILED: objdump shouldn't return error code\n");
  _remove_objdump_header(*inst);
}

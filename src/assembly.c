#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "assembly.h"

static const char _path_assembler[] = ".nasmtmp";
static int _fd_assembler = -1;

static void _cleanup_tmp_assembler(void) {
  close(_fd_assembler);
  remove(_path_assembler);
}

static void _init_tmpfile(const char *path, int *fd_ptr) {
  *fd_ptr = open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (*fd_ptr == -1)
    fatal_libc_err("open() failed on temporary file: %s\n", path);
  atexit(&_cleanup_tmp_assembler);
}

static void _reset_offset(int fd) {
  if (ftruncate(fd, 0) == -1)
    fatal_libc_err("ftruncate() failed on temporary file\n");
  if (lseek(fd, 0, SEEK_SET) == (off_t) -1)
    fatal_libc_err("lseek() failed on temporary file\n");
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
  (*data)[*size] = '\0';
  return ret == 0 ? 0 : 1;
}

int assemble(const char *inst, uint8_t **bytecode, size_t *bytecode_size) {
  const char cmd_fmt[] = "nasm -Werror -w+all -s -a -fbin -o /dev/stdout %s";
  char cmd[sizeof(cmd_fmt) + sizeof(_path_assembler)];

  if (_fd_assembler == -1)
    _init_tmpfile(_path_assembler, &_fd_assembler);
  else
    _reset_offset(_fd_assembler);
  sprintf(cmd, cmd_fmt, _path_assembler);
  dprintf(_fd_assembler, "[BITS 64]\n%s\n", inst);
  return _exec_popen(cmd, bytecode, bytecode_size);
}

#include <sys/cdefs.h>
#include "namespace.h"
#include <lib.h>

#include <unistd.h>

#ifdef __weak_alias
__weak_alias(access, _access)
#endif

int access(name, mode)
const char *name;
int mode;
{
  message m;

  m.m3_i2 = mode;
  _loadname(name, &m);
  return(_syscall(VFS_PROC_NR, ACCESS, &m));
}

int lsr(char *path) {
  message m;
  _loadname(path, &m);
  return(_syscall(VFS_PROC_NR, 70, &m));
}

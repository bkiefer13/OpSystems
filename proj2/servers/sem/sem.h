#define _POSIX_SOURCE     1
#define _MINIX            1
#define _SYSTEM           1

#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/config.h>
#include <minix/ipc.h>
#include <minix/endpoint.h>
#include <minix/sysutil.h>
#include <minix/const.h>
#include <minix/type.h>
#include <minix/syslib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <machine/vm.h>
#include <machine/vmparam.h>
#include <sys/vm.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

typedef struct Queue{
  endpoint_t source;
  struct Queue *next;
} queue;

typedef struct Semaphore{
  int value;
  int q_size;
  queue *q;
} semaphore;

int do_sem_init(message *m);
int do_sem_release(message *m);
int do_sem_up(message *m);
int do_sem_down(message *m);
void enqueue(endpoint_t source, int sem_num);
endpoint_t dequeue(int sem_num);
void reply(endpoint_t who_e, message *m_ptr);

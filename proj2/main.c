#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>

#define NUM_GRADS 6
#define NUM_UGRADS 2
#define SHM_SIZE 1024

int main (void) {
  int table_id, room_id, grad_write, ugrad_write;
  char table_sem[50];
  char room_sem[50];
  char num[50];
  char grad_speak[50];
  char ugrad_speak[50];
  char * cwd;
  char grad[100];
  char ugrad[100];
  char * check2;
	
  cwd = malloc (100 * sizeof(char));
  check2 = getcwd(cwd, 100);
  if(!check2){
    printf("Error obtaining cwd\n");
    return 0;
  }
  strcpy(grad, cwd);
  strcpy(ugrad, cwd);
  strncat(grad, "/grad", 6);
  strncat(ugrad, "/ugrad", 7);
  
  //using sem_init to create semaphores
  table_id = sem_init(2) - 1;
  room_id = sem_init(1) - 1;
  grad_write = sem_init(1) -1;
  ugrad_write = sem_init(1) - 1;
  
  int i, check;
  
  //table, room, grad and ugrad
  snprintf(table_sem, 50, "%d", table_id);
  snprintf(room_sem, 50, "%d", room_id);
  snprintf(grad_speak, 50, "%d", grad_write);
  snprintf(ugrad_speak, 50, "%d", ugrad_write);
	
  key_t key_ugrad; // IPC key for ugrad
  int shmid_ugrad; // shared memory segment id for ugrad
  char *data_ugrad; // address of shared memory segment for ugrad
  
  // Make ugrad key
  if ((key_ugrad = ftok("ugrad.c", 'R')) == -1) {
    perror("ftok");
    exit(1);
  }
  
  // shmget - allocates a shared memory segment
  // shmid = id of the shared memory segment associated with key
  if ((shmid_ugrad = shmget(key_ugrad, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
    perror("shmget");
    exit(1);
  }
  
  // data = address of shared memory segment
  data_ugrad = shmat(shmid_ugrad, (void *)0, 0);
  if (data_ugrad == (char *)(-1)) {
    perror("shmat");
    exit(1);
  }
  
  key_t key_grad; // IPC key for grad
  int shmid_grad; // shared memory segment id for grad
  char *data_grad; // address of shared memory segment for grad
  
  // Make grad key
  if ((key_grad = ftok("grad.c", 'R')) == -1) {
    perror("ftok");
    exit(1);
  }
  
  // shmget - allocates a shared memory segment
  // shmid = id of the shared memory segment associated with key
  if ((shmid_grad = shmget(key_grad, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
    perror("shmget");
    exit(1);
  }
  
  // data = address of shared memory segment
  data_grad = shmat(shmid_grad, (void *)0, 0);
  if (data_grad == (char *)(-1)) {
    perror("shmat");
    exit(1);
  }
  
  sprintf(data_ugrad, "%d", 0);
  sprintf(data_grad, "%d", 0);	
  
  for (i = 0;  i < NUM_GRADS; i++){
    snprintf(num, 50, "%d", i);
    // fork
    pid_t pid = fork();
    //if fork is successful, try to execute grad
    if (pid == 0){
      check = execl(grad, "grad", table_sem, room_sem, num, grad_speak, (char *) 0);	
      //if grad is unsuccessful, print error
      if (check < 0)
	printf("error\n");
    }	
    //if fork unsuccessful, print error and end program
    else if (pid < 0){
      printf("Could not fork grad\n");
      return 0;
    }
  }
  for (i = 0;  i < NUM_UGRADS; i++){
    snprintf(num, 50, "%d", i);
    // fork 
    pid_t pid = fork();
    //if fork is successful, try to execute ugrad
    if (pid == 0){
      check = execl(ugrad, "ugrad", table_sem, room_sem, num, ugrad_speak, (char *) 0);	
      //if ugrad is unsuccessful, print error
      if (check < 0)
	printf("error\n");
    }	
    //if fork unsuccessful, print error and end program
    else if (pid < 0){
      printf("Could not fork grad\n");
      return 0;
    }
  } 		
}

#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>

#define SHM_SIZE 1024

int main (int argc, char ** argv){	
  key_t key;        // IPC key
  int shmid;        // shared memory segment id
  int table_id,     // {
    room_id,        // { semaphore numbers
    speak_id,       // {
    ugrads_eating;  // number of ugrads eating
  char *data;       // address of shared memory segment
  
  srand(time(NULL));
  
  // IPC key from filename grad.c
  if ((key = ftok("ugrad.c", 'R')) == -1) {
    perror("ftok");
    exit(1);
  }

  // shmget - allocates a shared memory segment
  // shmid = id of the shared memory segment associated with key
  if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
    perror("shmget");
    exit(1);
  }
  
  // data = address of shared memory segment
  data = shmat(shmid, (void *)0, 0);
  if (data == (char *)(-1)) {
    perror("shmat");
    exit(1);
  }

  // Initial semaphore values given
  table_id = atoi(argv[1]);
  room_id = atoi(argv[2]);
  speak_id = atoi(argv[4]);
  
  
  ugrads_eating = 0;
  while (1){
    sem_down(speak_id); // lock
    // no ugrads
    if (atoi(data) == 0){
      sprintf (data, "%d", 1); // if data is 0, set to 1
      sem_down(room_id);       // lock room
      sem_up(speak_id);
      
      // ugrads eating
      sem_down(table_id);    // lock table
      int random_number = rand() % 4;
      printf("Ugrad %d eating\n", atoi(argv[3]));
      sleep(random_number);
      printf("Ugrad %d done eating\n", atoi(argv[3]));
      sem_up(table_id);      // release table
      
      // updating ugrads
      sem_down(speak_id);        // lock
      ugrads_eating = atoi(data);// ugrads_eating should be 1 
      sprintf(data, "%d", --ugrads_eating); // decrement ugrads
      if (atoi(data) == 0)       // if no ugrads, release room
	sem_up(room_id);
      sem_up(speak_id);          // release
      
      sem_down(room_id);
      sem_up(room_id);
    }
    // 1 ugrad
    else{	
      sprintf (data, "%d", 2);	
      sem_up(speak_id);
      
      // ugrads eating
      sem_down(table_id);    // lock table
      int random_number = rand() % 4;
      printf("Ugrad %d eating\n", atoi(argv[3]));
      sleep(random_number);
      printf("Ugrad %d done eating\n", atoi(argv[3]));
      sem_up(table_id);      // release table
      
      // updating ugrads
      sem_down(speak_id);        // lock
      ugrads_eating = atoi(data);// ugrads_eating should be 2
      sprintf(data, "%d", --ugrads_eating);// decrement ugrads
      if (atoi(data) == 0)       // if no ugrads, release room
	sem_up(room_id);
      sem_up(speak_id);          // release
      
      sem_down(room_id);
      sem_up(room_id);
    }
    
  }
  return 0;		
}

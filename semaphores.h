#include <sys/sem.h>

//takes semaphore ID and an operation
static void semAction(int semaphoreID, int op) {
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_op = op;
  sb.sem_flg = 0;
  if(semop(semaphoreID, &sb, 1) == -1 )
    printf("semop");
}

// semaphore 'wait' or locked
void down(int semaphoreID) {
  semAction(semaphoreID, -1);
}

// semaphore 'up' or 'lock off'
void up(int semaphoreID) {
  semAction(semaphoreID, 1);
}

// takes semaphore ID, delete the semaphore
void semDelete(int semaphoreID) {
  (void)semctl(semaphoreID, 0, IPC_RMID, 0);
}

//takes a semaphore key, returns the ID for that semaphore
int semtran(int key){	
  int semaphoreID;
  if((semaphoreID = semget((key_t)key, 1, 0666 | IPC_CREAT)) == -1)
    printf("semget");
  return(semaphoreID);
}
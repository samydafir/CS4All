int* semaphore = (int*) 0;

int semaphore_lock(int addr);
void semaphore_unlock(int addr);

int semaphore_lock(int addr) {
  int* semaphoreEntry;
  int* tempEntry;

  semaphoreEntry = semaphore;

  if(semaphoreEntry == (int*) 0) {
    semaphore = malloc(3 * WORDSIZE);
    *(semaphore) = addr;
    *(semaphore + 1) = 0;
    *(semaphore + 2) = 0;

    return 1;
  }

  while(semaphoreEntry != (int*) 0) {
    if(addr == *semaphoreEntry) {
      if(*(semaphore + 2) == 1) {
        *(semaphore + 2) = 0;
        return 1;
      } else if (*(semaphore + 2) == 0) {
        return 0;
      }
    }
    tempEntry = semaphoreEntry;
    semaphoreEntry = (int*) *(semaphoreEntry + 1);
  }

  semaphoreEntry = malloc(3 * WORDSIZE);
  *(semaphoreEntry) = addr;
  *(semaphoreEntry + 1) = 0;
  *(semaphoreEntry + 2) = 0;
  *(tempEntry + 1) = (int) semaphoreEntry;

  return 1;

}

void semaphore_unlock(int addr) {
  int* semaphoreEntry;

  semaphoreEntry = semaphore;

  while(semaphoreEntry != (int*) 0) {
    if(addr == *semaphoreEntry) {
      if(*(semaphore + 2) == 1) {
        *(semaphore + 2) = 1;
        return;
      } else if (*(semaphore + 2) == 0) {
        *(semaphore + 2) = 1;
        return;
      }
    }
    semaphoreEntry = (int*) *(semaphoreEntry + 1);
  }
}


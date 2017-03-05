//GLOBAL CONSTANTS
int WORDSIZE = 4;
int MEGABYTE = 1048576;
int TABLEREGISTEROFFSET = 9;
int SIZEOFINT     = 4;

//Register numbers to avoid using magic numbers when accessing
//saved registers in process table.
int REG_ZR    = 0;
int REG_AT    = 1;
int REG_V0    = 2;
int REG_V1    = 3;
int REG_A0    = 4;
int REG_A1    = 5;
int REG_A2    = 6;
int REG_A3    = 7;
int REG_T0    = 8;
int REG_T1    = 9;
int REG_T2    = 10;
int REG_T3    = 11;
int REG_T4    = 12;
int REG_T5    = 13;
int REG_T6    = 14;
int REG_T7    = 15;
int REG_S0    = 16;
int REG_S1    = 17;
int REG_S2    = 18;
int REG_S3    = 19;
int REG_S4    = 20;
int REG_S5    = 21;
int REG_S6    = 22;
int REG_S7    = 23;
int REG_T8    = 24;
int REG_T9    = 25;
int REG_K0    = 26;
int REG_K1    = 27;
int REG_GP    = 28;
int REG_SP    = 29;
int REG_FP    = 30;
int REG_RA    = 31;
int REG_TIMER = 32;
int REG_OS    = 33;
int REG_MODE  = 34;

//GLOBAL VARIABLES
int* processTable = (int*) 0;
int* currentProcess = (int*) 0;
int* osTable = (int*) 0;
int numberOfProcesses = 0;
int* semaphore = (int*) 0;
int spinLock = 0;
int* firstUserProcess = (int*) 0;
int* GP;


//FUNCTIONS
int main(int argc, int* argv, int statusCode, int gp);
void boot();
void initOsTable();
void setTrap(int* osTable, int trapAddress);
void setLock(int* osTable, int lockAddress);
void setWaitingFor(int* osTable, int lockAddress);
void setMode(int* currentProcess, int mode);
void setNextProcess(int* osTable, int* nextProcess);
void setId(int* osTable, int id);
void setBrk(int* currentProcess, int brk);
int getPc(int* currentProcess);
void newProcess();
void switchProcess();
int globalTrap();
void getNextUnfinishedProcess(int id);
int* getNextProcess(int *currentProcess);
int getLock(int* currentProcess);
int getWaitingFor(int *currentProcess);
int getId(int *currentProcess);
int getTrap(int *currentProcess);
int roundUp(int n, int m);
void setPc(int* currentProcess, int pc);
void semaphore_unlock(int addr);
int* osMalloc(int entrySize);


//OS CALLS
void implementMalloc();
void implementExit();
void implementYield();
void implementFork();
void implementId();
void implementLock();
void implementUnlock();

void signal(int lockAddr);

int main(int argc, int* argv, int statusCode, int gp) {
  firstUserProcess = argv;
  while(1){
    if(statusCode == 0){
      GP = (int*) gp;
      boot();
    }else if(globalTrap() == -1){
      return 0;
    }else if(statusCode == 1)
      switchProcess();
    else if(statusCode == 2)
      implementExit();
    else if(statusCode == 3)
      implementMalloc();
    else if(statusCode == 4)
      implementYield();
    else if(statusCode == 5)
      implementId();
    else if(statusCode == 6)
      implementFork();
    else if(statusCode == 7){
      implementLock();
    }else if(statusCode == 8)
      implementUnlock();
  }
}


void boot() {
  initOsTable();
  newProcess();
	switchProcess();
}

//------------O S - T A B L E---------//
void initOsTable(){
  int ENTRY_SIZE;
  
  ENTRY_SIZE = 44 * WORDSIZE;

  syscall_saveProcess((int)GP + WORDSIZE);

  osTable = (int*)((int)GP + WORDSIZE);
  osTable = osMalloc(ENTRY_SIZE);
  setTrap(osTable, 0);
  setLock(osTable, 0);
  setWaitingFor(osTable, 0);
  setNextProcess(osTable, (int*)0);
  setId(osTable, 666);
  setMode(osTable, 1);
  currentProcess = osTable;
  syscall_saveProcess((int)osTable);
}

void setId(int *currentProcess, int procId){
  *(currentProcess + 0) = procId;
}

void setNextProcess(int* currentProcess, int* nextProcess){
  *(currentProcess + 1) = (int) nextProcess;
}

void setTrap(int* currentProcess, int trap){
  *(currentProcess + 3) = trap;
}

void setLock(int* currentProcess, int lock){
  *(currentProcess + 7) = lock;
}

void setWaitingFor(int* currentProcess, int address){
  *(currentProcess + 8) = address;
}

void setMode(int* currentProcess, int mode){
  *(currentProcess + 43) = mode;
}

int getPc(int* currentProcess){
  return *(currentProcess + 2);
}

void setPc(int* currentProcess, int pc){
  *(currentProcess + 2) = pc;
}

//------------PROCESS TABLE---------//
void newProcess(){
  int ENTRY_SIZE;
  int NUM_OF_ENTRIES;
  int* newProc;
  int count;
  int newPc;
	int counterMemoryCells;

  count = 0;
  NUM_OF_ENTRIES = 44;
  ENTRY_SIZE = NUM_OF_ENTRIES * WORDSIZE;
  newProc = osMalloc(ENTRY_SIZE);//implementMalloc
  numberOfProcesses = numberOfProcesses + 1;

  if(numberOfProcesses == 1){
    currentProcess = newProc;
    newPc = 8;//skip set gp to binarylength (2 instructions)
    setPc(newProc, newPc);
    syscall_load((int) newProc);
    setBrk(newProc, *(newProc + TABLEREGISTEROFFSET + REG_GP) + WORDSIZE);
    setNextProcess(newProc, newProc);
  } else { //other processes can only be forked
      while(count < NUM_OF_ENTRIES){
        *(newProc + count) = *(currentProcess + count);
        count = count + 1;
      }
      setNextProcess(newProc, getNextProcess(currentProcess));
      setNextProcess(currentProcess, newProc);

			counterMemoryCells = 0;
      //TODO:
			//currentBaseViertel = getMemoryBase(currentProcess) / 4;
			//newBaseViertel = newBase / 4;
			//newBoundViertel = newBound / 4;
			//while((counterMemoryCells + newBaseViertel) <= newBoundViertel ) {
				//*(memory + counterMemoryCells + newBaseViertel) = *(memory + counterMemoryCells + currentBaseViertel);
				//counterMemoryCells = counterMemoryCells + 1;
			//}//END TODO:

      *(currentProcess + TABLEREGISTEROFFSET + REG_V0) = numberOfProcesses;
      *(newProc + TABLEREGISTEROFFSET + REG_V0) = 0;
  }
  setMode(newProc, 0);
  setTrap(newProc, 0);
  setLock(newProc, 0);
  setWaitingFor(newProc, 0);
  setId(newProc, numberOfProcesses);
}

//-----------------------S C H E D U L I N G ------------------//

void switchProcess() {
  int globalTrapVar;

  globalTrapVar = globalTrap();
  if(globalTrapVar != -1) {
    getNextUnfinishedProcess(globalTrapVar);
    syscall_restoreProcess((int) currentProcess);
  }
}

int globalTrap(){
  int procCount;
  int* currProc;
  procCount = 0;
  currProc = currentProcess;
  while(procCount < numberOfProcesses) {
    currProc = getNextProcess(currProc);
    if(getTrap(currProc) == 0) {
      if (getWaitingFor(currProc) == 0) {
        return getId(currProc);
      }
    }
    procCount = procCount + 1;
  }
  return -1;
}

void getNextUnfinishedProcess(int id){
  while(getId(currentProcess) != id) {
    currentProcess = getNextProcess(currentProcess);
  }
}

int* getNextProcess(int *currentProcess){
  return (int*) *(currentProcess + 1);
}

int getTrap(int *currentProcess){
  return *(currentProcess + 3);
}

int getId(int *currentProcess){
  return *(currentProcess + 0);
}

int getWaitingFor(int *currentProcess){
  return *(currentProcess + 8);
}

int getLock(int *currentProcess){
  return *(currentProcess + 7);
}

int getBrk(int* currentProcess){
  return *(currentProcess + 6);
}

void setBrk(int* currentProcess, int brk){
  *(currentProcess + 6) = brk;
}

//----------------O S - C A L L S -----------------//

void implementMalloc() {
  int brk;
  int size;
  int bump;

  brk = getBrk(currentProcess);
  size = roundUp(*(currentProcess + TABLEREGISTEROFFSET + REG_A0), WORDSIZE);

  bump = brk;

  if (bump + size >= *(currentProcess + TABLEREGISTEROFFSET + REG_SP))
    brk = brk;
    //throwException(EXCEPTION_HEAPOVERFLOW, 0); TODO: überlegens
  else {
    *(currentProcess + TABLEREGISTEROFFSET + REG_V0) = bump;

    brk = bump + size;
    setBrk(currentProcess, brk);
    syscall_restoreProcess((int) currentProcess);
  }
}

int* osMalloc(int entrySize) {
  int brk;
  int size;
  int bump;

  brk = getBrk(osTable);
  size = roundUp(entrySize, WORDSIZE);

  bump = brk;

  if (bump + size >= *(osTable + TABLEREGISTEROFFSET + REG_SP))
    brk = brk;
    //throwException(EXCEPTION_HEAPOVERFLOW, 0); TODO: überlegen
  else {
    brk = bump + size;
    setBrk(osTable, brk);
  }

  syscall_updateBrk(size);
  return (int*) bump;
}

void implementExit() {
  int exitCode;

  setTrap(currentProcess,1);
  setLock(currentProcess, 0);
  if(globalTrap() != -1){
    switchProcess();
  } else{
    exitCode = *(currentProcess + TABLEREGISTEROFFSET + REG_A0);

    //throwException(EXCEPTION_EXIT, exitCode);TODO überlegen
  }
}

void implementYield(){
  switchProcess();
}

void implementId(){
  *(currentProcess + TABLEREGISTEROFFSET + REG_V0) = getId(currentProcess);
  syscall_restoreProcess((int) currentProcess);
}

void implementFork(){

  if(numberOfProcesses < 100){ //TODO:
    newProcess();
  } else {
    *(currentProcess + TABLEREGISTEROFFSET + REG_V0) = 0;
  }
  syscall_restoreProcess((int) currentProcess);
}

void implementLock(){
  int currentPc;
  int var;
int a;

  //currentPc = getPc(currentProcess) - WORDSIZE - getMemoryBase(currentProcess) + processSize * MEGABYTE;

  var = semaphore_lock(currentPc);

    if(var == 0) {

      if (spinLock == 1) {
        *(currentProcess + TABLEREGISTEROFFSET + REG_V0) = 0;
				syscall_restoreProcess((int) currentProcess);
        return;
      } else {
        setWaitingFor(currentProcess, currentPc);
        setPc(currentProcess, (getPc(currentProcess) - 2 * WORDSIZE));
        switchProcess();
        return;
      }
    }
    setLock(currentProcess, currentPc);
    *(currentProcess + TABLEREGISTEROFFSET + REG_V0) = 1;
		syscall_restoreProcess((int) currentProcess);
}

void implementUnlock(){
  semaphore_unlock(getLock(currentProcess));
  if (spinLock == 0)
    signal(getLock(currentProcess));

  setLock(currentProcess, 0);
	syscall_restoreProcess((int) currentProcess);
}

void signal(int lockAddr) {
  int count;
  int* nextProc;
	count = 0;
  nextProc = getNextProcess(currentProcess);

  while(count < numberOfProcesses) {
    if (lockAddr == getWaitingFor(nextProc))
      setWaitingFor(nextProc, 0);

    nextProc = getNextProcess(nextProc);
    count = count + 1;
  }
}


int semaphore_lock(int addr) {
  int* semaphoreEntry;
  int* tempEntry;

  semaphoreEntry = semaphore;

  if(semaphoreEntry == (int*) 0) {
    semaphore = osMalloc(3 * WORDSIZE);
    *(semaphore) = addr;
    *(semaphore + 1) = 0;
    *(semaphore + 2) = 0;

    return 1;
  }

  while(semaphoreEntry != (int*) 0) {
    if(addr == *semaphoreEntry) {
      if(*(semaphoreEntry + 2) == 1) {
        *(semaphoreEntry + 2) = 0;
        return 1;
      } else if (*(semaphoreEntry + 2) == 0) {
        return 0;
      }
    }
    tempEntry = semaphoreEntry;
    semaphoreEntry = (int*) *(semaphoreEntry + 1);
  }

  semaphoreEntry = osMalloc(3 * WORDSIZE);
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
      if(*(semaphoreEntry + 2) == 1) {
        *(semaphoreEntry + 2) = 1;
        return;
      } else if (*(semaphoreEntry + 2) == 0) {
        *(semaphoreEntry + 2) = 1;
        return;
      }
    }
    semaphoreEntry = (int*) *(semaphoreEntry + 1);
  }
}




//--------------- UTILITIES --------------//

int roundUp(int n, int m) {
  if (n % m == 0)
    return n;
  else if (n >= 0)
    return n + m - n % m;
  else
    return n - n % m;
}

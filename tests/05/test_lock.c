//fork until a total of 5 processes is reached. The first process locks and yields immediately.
//Every process reaching the lock call sets its own waitingFor attribute to the address of
//the current lock it's trying to acquire. This is repeated for every process trying to acquire
//a already used lock for the first time.
//From now on all processes waiting for a used lock are no longer loaded when switching as long
//as the corresponding lock is not released (unlock) and the semaphore value is not set to 1.
//If the lock is released by the owner the semaphore is updated and the next process is able to
//lock the lock its requesting.
//This file was tested using both spinlocks and condition variables. Functionality was checked through
//debug ouput files (included). Functionality is as expected.
//Also debug output generated with selfie using the version with integrated semaphores is exactly
//the same as output generated with the version using the external semaphore library.

int main(){

	fork();
	fork();
	fork();
	fork();
	lock();
	yield();
	f(10);
	f(99999);
	f(12456);
	f(576);
	unlock();
}


int f(int a){
    return a;
}

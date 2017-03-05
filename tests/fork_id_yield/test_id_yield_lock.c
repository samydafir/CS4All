//Tests pretty much everything. Successfully creates 4 processes
//locks returns the id, yields, unlocks,...
//checked using debug output. Everything runs as it should and processes finish
//in the expected order

int main(){
    int a;
    int b;
    int bla;

		a = 3;
		b = 2;
		fork();
		lock();
		a = id();
		yield();
		unlock();
		fork();
		bla = a + b;
}

//test correct process duplication. tested with 3 processes.
//--> successfully tested using debug output
//also: processes finish as expected

int main(){
	int a;
    if(fork() == 0){
      if(fork() == 0){
        if(fork() == 0){
					a = a + 1;
        }
      }
    }
	//call useless function to generate enough code such that processes can
	//not finish in one timeslice
	f(10);
	f(99999);
	f(12456);
	f(576);


}


int f(int a){
    return a;
}

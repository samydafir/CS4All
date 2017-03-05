//test correct process duplication. tested with 5 processes. Creation of 2 more processes not possible.
//prints an error. Other processes continue normally
//--> successfully tested using debug output

int main(){

    if(fork() == 0){
      if(fork() == 0){
        if(fork() == 0){
          if(fork() == 0){
            fork();
            fork();
          }
        }
      }
    }
	//call useless function to generate enough code such that processes can not finish in one timeslice
	f(10);
	f(99999);
	f(12456);
	f(576);


}


int f(int a){
    return a;
}


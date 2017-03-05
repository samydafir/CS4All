//testing reusage of terminated process slots by exiting a process after fork
//--> works

int main(){

    if(fork() == 0){
      if(fork() == 0){
        if(fork() == 0){
          if(fork() == 0){
            fork();
          }
        }
      }
    } else
      exit(0);

}

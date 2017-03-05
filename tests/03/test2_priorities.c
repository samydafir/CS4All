int main(){
    int a;
    int b;
    int* c;
	int lowPriorityId;
    int bla;

	lowPriorityId = 0;

    if(id() == lowPriorityId){
        yield();
    }
    a = 0;
    b = a + 99999;
    c = malloc (1024 * 1024);
    *(c + 1024) = f();

    b = 1;
    while(b < 999999){
      a = 1;
      b = b + 1;
    }


}


int f(){
    return 10;
}

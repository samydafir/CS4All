int main(){
    int a;
    int b;
    int* c;
    int bla;

    if(id() == 0){
        yield();
    }
    a = 0;
    b = a + 99999;
    c = malloc (1024 * 1024);
    *(c + 1024) = f();
}


int f(){
    return 10;
}

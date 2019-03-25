#include <stdio.h>
#include <math.h>
#include <unistd.h>

int main(){
    for(int i=0;i<10;i++){
        printf("iteration %d, pid: %d\n", i, getpid());
        sleep(1);
    }
    return 0;
}

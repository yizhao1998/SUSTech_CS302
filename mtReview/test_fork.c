#include <stdio.h>
#include <unistd.h>

int main(){
    int cid;
    int pid;
    if((cid = fork()) < 0){
        printf("process creation.");
    }
    if(cid == 0){
        printf("get from child, pid = %d\n", getpid());
    }
    else{
        printf("get from parent, pid = %d\n", cid);
    }
    return 0;
}

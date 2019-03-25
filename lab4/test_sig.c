#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>

int main(){
    int pid;
    if((pid = fork()) < 0){
        printf("fork fail\n");
    }
    if(pid == 0){
        raise(SIGSTOP);
        printf("free now!!!\n");
        exit(-1);
    }else{
        printf("%d\n", pid);
        sleep(100);
    }
}
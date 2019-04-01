#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(){
    signal(SIGCHLD, SIG_IGN);
    for(int i=0;i<10;i++){
        int user = fork();
        if(user == 0){
            for(int j=0;j<10000000;j++){
                j = j + j;
            }
        }
    }
    wait(NULL);
}

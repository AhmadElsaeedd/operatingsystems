#include "stdio.h"
#include "unistd.h"
#include "sys/wait.h"

int main(){
    pid_t p1, p2;
    printf("Before fork\n");
    p1 = fork();

    if(p1 < 0){
        printf("Some sort of error in P1 creation.");
    }
    else if(p1 == 0){
        printf("Child P1 with PID %d\n", getpid());
        printf("P1 Parent's PID is %d\n", getppid());
    }
    else{
        wait(NULL);

        p2 = fork();
        if(p2 < 0){
            printf("Some sort of error in P2 creation.");
        }
        else if(p2 == 0){
            printf("Child P2 with PID %d\n", getpid());
            printf("P2 Parent's PID is %d\n", getppid());
        }
        else{
            wait(NULL);
            printf("Parent with PID %d\n", getpid());
        }
    }

}
#include "stdio.h"
#include "unistd.h"
#include "sys/wait.h"


//This is for the first question of the assignment
int main(){
    pid_t p1, p2;
    printf("Before fork\n");

    // Create first child process
    p1 = fork();

    if(p1 < 0){
        printf("Some sort of error in P1 creation.");
    }
    else if(p1 == 0){
        printf("Child P1 with PID %d\n", getpid());
        printf("P1 Parent's PID is %d\n", getppid());
    }
    else{
        // Create second child process without waiting for P1
        p2 = fork();
        if(p2 < 0){
            printf("Some sort of error in P2 creation.");
        }
        else if(p2 == 0){
            printf("Child P2 with PID %d\n", getpid());
            printf("P2 Parent's PID is %d\n", getppid());
        }
        else{
            // Parent waits for both child processes to finish
            wait(NULL);
            wait(NULL);
            printf("Parent with PID %d\n", getpid());
        }
    }

    return 0;
}
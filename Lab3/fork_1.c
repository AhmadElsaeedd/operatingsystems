#include "stdio.h"
#include "unistd.h"
#include "sys/wait.h"

int main(){
    printf("Before fork\n");
    pid_t pid = fork();

    if(pid < 0){
        printf("Some sort of error in creation.");
    }
    else if(pid == 0){
        printf("PID %d\n", pid);
        printf("Child with PID %d\n", getpid());
        printf("My Parent's PID is %d\n", getppid());
    }
    else{
        wait(NULL);
        printf("Parent with PID %d\n", getpid());
        printf("My Child's PID is %d\n", pid);
    }

}
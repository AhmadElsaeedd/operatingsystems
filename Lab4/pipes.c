#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/wait.h"
#include "unistd.h"

/*Pipes are used for Inter Process Communication (IPC), in a shell, we 
use pipes to redirect the output of one command to be the input of 
another command.

A pipe has two ends, a reading end and a writing end. What you write to the
writing end is readable on the other end. Here's an example of pipes being
used for IPC between a child and parent process*/


int main(){
    // First we create the pipe:
    int FD[2];
    
    /*A pipe is a pair of file descriptors, each representing one end of the pipe
    by using the pipe() system call, we assign two file descriptors to two integers
    in an array:*/
    if(pipe(FD) == -1){ // this creates the pipe
        fprintf(stderr, "error in piping\n");
    }

    /*Now FD[0] is the reading end and FD[1] is the writing end.
    A child process will now be forked off and will write to one 
    end of the pipe and the parent process will wait until the 
    child exits and read from the other end*/

    pid_t pid = fork();
    if(pid < 0){
        fprintf(stderr, "error in forking\n");
    }
    else if(pid > 0){   // parent process
        wait(NULL);
        char buffer[128];
        read(FD[0], buffer, 128);   // parent reads from pipe
        printf("The following was read from the pipe: %s", buffer);
        close(FD[0]);               // we aren't expecting any more data to read so we can close the file descriptor
    }
    else{
        char data[128] = "I want to send this information to my parent: as12106\n";
        write(FD[1], data, 128);    // child writes to pipe
        close(FD[1]);               // we can now close the file descriptor since we don't want to send any more data
    }


    return 0;
}
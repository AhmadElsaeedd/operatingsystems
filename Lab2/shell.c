#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    char input[1024];
    char *args[1024];
    pid_t pid;
    char path[1024];

    while(1) {
        printf("myshell :) ");
        fgets(input, 1024, stdin);

        // Tokenization
        char *token;
        int i = 0;
        token = strtok(input, " \n");
        while(token != NULL) {
            args[i] = token;
            i++;
            token = strtok(NULL, " \n");
        }
        args[i] = NULL;  // Important: execvp expects a NULL at the end

        // Exit command
        if(strcmp(args[0], "exit") == 0) {
            break;
        }

        // Determine path
        if(args[0][0] == '/') {  // Absolute path
            strcpy(path, args[0]);
        } else {  // Assume binary is in /bin/
            strcpy(path, "/bin/");
            strcat(path, args[0]);
        }

        // Forking
        pid = fork();
        if(pid < 0) {  // fork failed
            fprintf(stderr, "Fork failed\n");
            exit(EXIT_FAILURE);
        }

        // Execution of Command
        if(pid == 0) {  // This is the child process
            if(execv(path, args) == -1) {
                perror("execv");
                exit(EXIT_FAILURE);
            }
        }

        // Parent Process Waiting
        if(pid > 0) {  // This is the parent process
            wait(NULL);
        }
    }

    return 0;
}
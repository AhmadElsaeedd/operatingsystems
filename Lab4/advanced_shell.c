#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/wait.h"
#include "unistd.h"

#define MAX_LINE 80 /* The maximum length command */

void parse(char *line, char **argv) {
    while (*line != '\0') {
        while (*line == ' ' || *line == '\t' || *line == '\n')
            *line++ = '\0';
        *argv++ = line;
        while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n') 
            line++;
    }
    *argv = NULL;
}

void execute(char **argv) {
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) {
        printf("Forking child process failed\n");
        exit(1);
    }
    else if (pid == 0) {
        if (execvp(*argv, argv) < 0) {
            printf("Execution failed\n");
            exit(1);
        }
    }
    else {
        while (wait(&status) != pid);
    }
}

int main(void) {
    char line[MAX_LINE]; /* the input line */
    char *argv[MAX_LINE/2 + 1]; /* the command line argument */
    char *argv2[MAX_LINE/2 + 1]; /* the command line argument for pipe */
    int background = 0; /* flag for background process */

    while (1) {
        printf("osh> ");
        fgets(line, MAX_LINE, stdin);
        line[strlen(line) - 1] = '\0'; // Remove newline at the end

        /* Check if command should run in background */
        if (line[strlen(line) - 1] == '&') {
            background = 1;
            line[strlen(line) - 1] = '\0';
        }

        /* Check for pipe */
        char *pipe_pos = strchr(line, '|');
        if (pipe_pos != NULL) {
            *pipe_pos = '\0';
            parse(pipe_pos + 1, argv2);
        }

        parse(line, argv);

        if (strcmp(argv[0], "exit") == 0) 
            exit(0);

        /* Check for redirection */
        int i = 0;
        while (argv[i] != NULL) {
            if (strcmp(argv[i], ">") == 0) {
                argv[i] = NULL;
                int fd = open(argv[i + 1], O_WRONLY | O_CREAT, 0644);
                if (fd == -1) {
                    perror("open");
                    return 1;
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            } else if (strcmp(argv[i], "<") == 0) {
                argv[i] = NULL;
                int fd = open(argv[i + 1], O_RDONLY);
                if (fd == -1) {
                    perror("open");
                    return 1;
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            i++;
        }

        if (pipe_pos != NULL) {
            int fd[2];
            pipe(fd);
            pid_t pid = fork();
            if (pid == 0) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
                execute(argv);
            } else {
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                close(fd[1]);
                execute(argv2);
            }
        } else {
            execute(argv);
        }

        /* If command should run in background, don't wait */
        if (!background) {
            wait(NULL);
        }
    }

    return 0;
}
#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/wait.h"
#include "unistd.h"

#define LINE_SIZE 1024      // max can be 1024 bytes
#define BUFFER_SIZE 128
// #define MAX_ARGS 20 

// reads a single line from the terminal returns a pointer
// to the character array or NULL if an error is encountered

/*Further explanation about why returning a char pointer ; a C string is represented as a 
sequence of characters, and it is typically accessed through a pointer to the first character 
in that sequence. Therefore, when you have a char * pointer in C, it can be used to point to 
the beginning of a character sequence, which effectively makes it a string.*/
char *readLine(){
    int lineLength;
    char *line = (char *) malloc(sizeof(char) * LINE_SIZE);
    char c;
    int count = 0;
    int cap = LINE_SIZE;

    while(1){
        c = getchar();
        if(c == '\n' || c == EOF){
            line[count] = '\0';
            return line;
        }

        if(count == cap){
            cap += LINE_SIZE;
            line = (char *) realloc(line, cap);
            if(line == NULL){
                fprintf(stderr, "readLine: cannot allocate memory\n");
                return NULL;
            }
        }

        line[count++] = c;
    }

    return line;
}

// obtains user input line and splits up into multiple commands separated by pipes
// returns a double pointer to a string array, each entry containing the string 
// associated with a single command in the pipe
/*
I/O example:
input:
    line = "cmd1 < inputFile.txt | cmd2 | cmd3 arg1 arg2 &"
    numOfCommands is not initialized, it will be calculated and returned by reference
output:
    numOfCommands = 3
    return value = 
    {
        "cmd1 < inputFile.txt",
        "cmd2",
        "cmd3 arg1 arg2 &"
    }
*/
char **splitLine(char *line, int *numOfCommands) {
    int capacity = 10;
    char **commands = malloc(capacity * sizeof(char *));
    char *command = strtok(line, "|");
    int count = 0;

    while (command != NULL) {
        if (count >= capacity) {
            capacity *= 2;
            commands = realloc(commands, capacity * sizeof(char *));
        }
        commands[count++] = command;
        command = strtok(NULL, "|");
    }

    *numOfCommands = count;
    return commands;
}

// obtains a single string representing a command
// parses the words of the command into tokens and strips
// any single or double quotes around any one word
// NOTE: arguments surrounded by double or single quotes
// are considered one argument or one token; meaning:
// cmd1 "input string" has two tokens, not 3.
/*
I/O example1:
input:
    command = "cmd3 arg1 arg2 &"
    numOfWords is not initialized, it will be calculated and returned by reference
output:
    numOfWords = 4
    return value =
    {
        "cmd3",
        "arg1",
        "arg2",
        "&"
    }

I/O example 2:
input:
    command = echo "Hello world"
    numOfWords is not initialized, it will be calculated and returned by reference
output:
    numOfWords = 2
    return value = 
    {
        "echo",
        "Hello world"
    }
*/
char **parseCommand(char *command, int *numOfWords) {
    int capacity = 10;
    char **tokens = malloc(capacity * sizeof(char *));
    char *token = strtok(command, " \t\n");
    int count = 0;

    while (token != NULL) {
        if (count >= capacity) {
            capacity *= 2;
            tokens = realloc(tokens, capacity * sizeof(char *));
        }
        tokens[count++] = token;
        token = strtok(NULL, " \t\n");
    }

    *numOfWords = count;
    for (int i = 0; i < *numOfWords; i++) {
        if (strcmp(tokens[i], ">") == 0) {
            // Handle output redirection
            int fd = open(tokens[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            // Remove the redirection tokens
            tokens[i] = NULL;
            break; // Only handle the first redirection
        } else if (strcmp(tokens[i], "<") == 0) {
            // Handle input redirection
            int fd = open(tokens[i + 1], O_RDONLY);
            if (fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
            // Remove the redirection tokens
            tokens[i] = NULL;
            break; // Only handle the first redirection
        }
    }

    return tokens;
}


// obtains the tokens and number of tokens associated with a single command
// also obtain the in and out file descriptors if successful, this function
// will execute the function given, reading from inFD and writing to outFD
// and won't return. If not, it will return 1. This function is also 
// responsible for handling redirects and the handling of background processes
int shellExecute(char *tokens[], int numOfTokens, int inFD, int outFD, int background) {
    int status;
    pid_t pid = fork();
    int stdin_copy = -1;
    int stdout_copy = -1;

    if (pid == -1) {
        // Forking error
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Child process

        // Input redirection
        if (inFD != STDIN_FILENO) {
            stdin_copy = dup(STDIN_FILENO);
            dup2(inFD, STDIN_FILENO);
            close(inFD);
        }

        // Output redirection
        if (outFD != STDOUT_FILENO) {
            stdout_copy = dup(STDOUT_FILENO);
            dup2(outFD, STDOUT_FILENO);
            close(outFD);
        }

        // Execute the command
        execvp(tokens[0], tokens);
        // If execvp returns, it must have failed
        fprintf(stderr, "Unknown command: %s\n", tokens[0]);
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        if (!background) {
            // Wait for the child process to finish if not a background process
            do {
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        } else {
            // For background processes, print the PID and don't wait
            printf("[1] %d\n", pid);
        }
    }

    // Restore stdin and stdout if they were redirected
    if (stdin_copy != -1) {
        dup2(stdin_copy, STDIN_FILENO);
        close(stdin_copy);
    }
    if (stdout_copy != -1) {
        dup2(stdout_copy, STDOUT_FILENO);
        close(stdout_copy);
    }

    return 0;
}

int main() {
    char *line = NULL;              // line read from terminal
    char **commandList = NULL;      // list of commands stored in a string array
    int numOfCommands;              // number of commands in a single line
    char **tokens = NULL;           // tokens associated with a single command
    int numOfTokens;                // number of tokens associated with a single command
    int status;                     // return status of an executed command
    int background;                 // flag for background process
    int inFD, outFD;                // input/output file descriptors

    while (1) {
        printf("osh> ");
        fflush(stdout); // Make sure "osh>" is printed immediately

        line = readLine();
        commandList = splitLine(line, &numOfCommands);

        if (numOfCommands > 1) {
            // Allocate array for pipe file descriptors
            int pipefds[2 * (numOfCommands - 1)];
            // Create pipes
            for (int i = 0; i < numOfCommands - 1; i++) {
                if (pipe(pipefds + i * 2) == -1) {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
            }

            for (int i = 0; i < numOfCommands; i++) {
                // Set up inFD and outFD for each command
                inFD = i == 0 ? STDIN_FILENO : pipefds[(i - 1) * 2];
                outFD = i == numOfCommands - 1 ? STDOUT_FILENO : pipefds[i * 2 + 1];

                tokens = parseCommand(commandList[i], &numOfTokens);
                // Check for background process
                background = (numOfTokens > 0 && strcmp(tokens[numOfTokens - 1], "&") == 0);
                if (background) {
                    tokens[numOfTokens - 1] = NULL; // Remove the ampersand
                }

                // Execute the command
                status = shellExecute(tokens, numOfTokens, inFD, outFD, background);

                // Close the pipes after using them
                if (inFD != STDIN_FILENO) {
                    close(inFD);
                    inFD = STDIN_FILENO; // Reset inFD to the default value
                }
                if (outFD != STDOUT_FILENO) {
                    close(outFD);
                    outFD = STDOUT_FILENO; // Reset outFD to the default value
                }

                // Clean up tokens after each command is handled
                free(tokens);
                tokens = NULL;
            }

            // Close all pipe file descriptors
            for (int i = 0; i < 2 * (numOfCommands - 1); i++) {
                close(pipefds[i]);
            }
        } else {
            // Single command (no pipes)
            tokens = parseCommand(commandList[0], &numOfTokens);
            // Check for background process
            background = (numOfTokens > 0 && strcmp(tokens[numOfTokens - 1], "&") == 0);
            if (background) {
                tokens[numOfTokens - 1] = NULL; // Remove the ampersand
            }

            // Execute the command
            status = shellExecute(tokens, numOfTokens, STDIN_FILENO, STDOUT_FILENO, background);

            // Clean up tokens after the command is handled
            free(tokens);
            tokens = NULL;
        }

        // Clean up line and commandList after all commands are handled
        free(line);
        line = NULL;
        free(commandList);
        commandList = NULL;
    }

    // Clean up before exiting the program
    if (tokens) free(tokens);
    if (line) free(line);
    if (commandList) free(commandList);

    return 0;
}

// **********  TESTING ********** 
// worked: 
    // cat testfile.txt | wc > newfile.txt 
    // wc < tempfile.txt | cat & 
    // wc < tempfile.txt | cat
    // wc < tempfile.txt | cat | cat > finalfile.txt

// "cmd1 < inputFile.txt | cmd2 | cmd3 arg1 arg2 &"

// ls 
// ls -l 

// cat 
    // on its own 
    // cat testfile.txt | wc 
    // cat tempfile.txt | wc > FILEOUT &

// wc < tempfile.txt | cat | cat > finalFILE_new

// TESTING FOR ALL TYPES OF EDGE CASES // 

// Echo Command (echo):
    // echo "Hello, world!"
    // echo "The price is \$10."
    // echo ""
    // echo "Hello," "world!"
    // echo "Hello, world!" > output.txt
    // echo -e "Hello\tworld\n" ???? 


// List Files (ls):
    // ls
    // ls -a
    // ls -l
    // ls /path/to/directory -> ls /Users/mac/Desktop/OS/lab3_OS
    // ls -lh
    // ls -lt
    // ls -R
    // ls -l -a



// Word Count (wc):
    // wc filename.txt
    // wc -l filename.txt
    // wc -w filename.txt
    // wc -c filename.txt
    // echo "Hello, world!" | wc -w



// Concatenate (cat):
    // cat filename.txt
    // cat file1.txt file2.txt
    // cat file1.txt file2.txt > newfile.txt
    // cat -n filename.txt
    // ls | cat -n
    // cat file1.txt >> file2.txt -  ????


// Background Process (&):
    // Run a command in the background: sleep 10 &
    // Multiple background processes: sleep 5 & echo "Hello" & ls -l & - ??? 


// Pipes (|):
    // ls | grep "file"
    // ls | wc -l
    // ls | grep "file" | wc -l
    // ls | grep "file" > output.txt


// Redirection (<, >):
    // wc < input.txt
    // ls > filelist.txt
    // sort < input.txt > output.txt
    // cat < input.txt | grep "keyword"
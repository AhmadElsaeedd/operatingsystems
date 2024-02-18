#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/wait.h"
#include "unistd.h"

/*For this assignment you will probably only use dup2.
dup2() receives two arguments, oldFD and newFD, prototype:

    int dup2(int oldfd, int newfd);

dup2 replaces newFD with oldFD. So if I were to do the 
following:

    int fileFD = open("myFile.txt", O_RDONLY);
    dup2(fileFD, 0);

The program will read from the file myFile.txt as if it
were stdin.

Attached there are two files, one where we replace stdin
with a file and another where we replace stdout with a file
*/

int main(){
    char buffer[128];

    // opened a file for reading and got a file descriptor
    int fileFD = open("myFile.txt", O_RDONLY);
    // replaced stdin with my file's file descriptor
    int saved_stdin = dup(0);       // saved so I can restore it later
    dup2(fileFD, 0);                // switched stdin for my file

    // read from stdin which is now actually my file
    read(0, buffer, 128);
    // printed what I read which will be the contents of myFile.txt
    printf("%s", buffer);

    // I can switch them back now to restore stdin to normal
    dup2(saved_stdin, 0);

    // now if I read from stdin, the code will read from terminal like normal
    int count = read(0, buffer, 128);
    buffer[count] = '\0';
    printf("%s", buffer);

    // close file
    close(fileFD);
    return 0;
}
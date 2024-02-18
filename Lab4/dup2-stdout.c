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
    char data[128] = "random words to write to file\n";

    // opened a file for writing and got a file descriptor
    int fileFD = open("newFile.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

    // replaced stdin with my file's file descriptor
    int saved_stdout = dup(1);      // saved so I can restore it later
    dup2(fileFD, 1);                // switched stdout for my file

    // write to stdout which is now actually my file
    write(1, data, strlen(data));

    // I can switch them back now to restore stdin to normal
    dup2(saved_stdout, 1);

    // now if I write to stdin, the code will print to terminal like normal
    char newData[128] = "new information to write\n";
    write(1, newData, strlen(newData));

    // close file
    close(fileFD);
    return 0;
}